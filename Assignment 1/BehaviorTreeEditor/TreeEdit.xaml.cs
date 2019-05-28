using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace BehaviorTreeEditor
{
	/// <summary>
	/// Interaction logic for TreeEdit.xaml
	/// </summary>
	public partial class TreeEdit : UserControl
	{
		// variables

		// node list (right column)
		public List<BTNode> _nodes;
		// mouse start position when dragging
		public Point startPoint;
		// tree list (left column), used to construct BTTree
		public List<BTNode> _tree;
		// tree invalid data
		public List<TreeValidateData> _invalid_data;
		// which invalid data to change background color
		public int _invalid_index;

		// methods 

		public TreeEdit()
		{
			InitializeComponent();

			InitializeMembers();


			LoadNodes();

			ListBoxBinding();
			OutputMessageBinding();

			//this.Loaded += (o, e) => {
			
			//    LoadNodes();

			//    ListBoxBinding();
			//    OutputMessageBinding();
			//};
			


		}

		// Initialize all member variables
		private void InitializeMembers()
		{
			_invalid_index = -1;
			_nodes = new List<BTNode>();
			_tree = new List<BTNode>();
			_invalid_data = new List<TreeValidateData>();
		}

		// update treename label
		public void UpdateTreeNameLabel()
		{
			if (App._global._treeName == null || App._global._treeName == "")
				TreeNameLabel.Content = "(Empty)";
			else
				TreeNameLabel.Content = App._global._treeName;
		}

		// Load node info
		public void LoadNodes()
		{
			string nodesfile = App._projectDir + App._resourcesDir + App._nodesFileName;
			string[] lines = System.IO.File.ReadAllLines(nodesfile);
			int index = 0;

			foreach (string line in lines)
			{
				// get node type
				BTNodeType type = BTNodeType.None;

				if (line.StartsWith("REGISTER_CONTROLFLOW"))
					type = BTNodeType.Composite;
				else if (line.StartsWith("REGISTER_DECORATOR"))
					type = BTNodeType.Decorator;
				else if (line.StartsWith("REGISTER_LEAF"))
					type = BTNodeType.Leaf;

				if (type != BTNodeType.None)
				{
					// get node name
					int node_startindex = line.IndexOf("(") + 1;
					int node_endindex = line.IndexOf(",") - 1;
					string nodename = line.Substring(node_startindex, node_endindex - node_startindex + 1);

					// get node summary
					int summary_startindex = line.IndexOf("\"") + 1;
					int summary_endindex = line.LastIndexOf("\"") - 1;
					string nodesummary = line.Substring(summary_startindex, summary_endindex - summary_startindex + 1);

					_nodes.Add(new BTNode() { _name = nodename, _summary = nodesummary, _type = type, _index = index++ });
				}
			}
		}

		// Bind nodes into listbox
		private void ListBoxBinding()
		{
			BTNodes.ItemsSource = _nodes;
		}

		// Bind nodes into output message
		public void OutputMessageBinding()
		{
			OutputMessage.ItemsSource = null;
			OutputMessage.ItemsSource = _invalid_data;
		}

		// Helper to search up the VisualTree
		private static T FindAnchestor<T>(DependencyObject current)
			where T : DependencyObject
		{
			do
			{
				if (current is T)
				{
					return (T)current;
				}
				current = VisualTreeHelper.GetParent(current);
			}
			while (current != null);
			return null;
		}

		// add node to _tree list
		private void AddNodeToTree(BTNode node)
		{
			if (_tree.Count == 0)
			{
				node._depth = 0;
				node._index = 0;
			}
			else
			{
				node._depth = _tree.Last()._depth;
				if (node._depth == 0)
					node._depth = 1;
				node._index = _tree.Count;
			}

			_tree.Add(node);
		}

		// insert node into the tree
		private void InsertNodeToTree(BTNode node, int index)
		{
			node._depth = _tree[index]._depth;
			if (node._depth == 0)
				_tree[index]._depth = 1;
			node._index = index;

			_tree.Insert(index, node);

			UpdateTreeIndex();
		}

		// remove node from tree
		private void RemoveNodeFromTree(int index)
		{
			_tree.RemoveAt(index);

			UpdateTreeIndex();
		}

		// update _index value on tree
		private void UpdateTreeIndex()
		{
			for (int i = 0; i < _tree.Count; ++i)
			{
				_tree[i]._index = i;
			}
		}

		public void EmptyTree()
		{
			_tree.Clear();
			UpdateTreeIndex();
			UpdateBTTree();

			_invalid_data.Clear();
			AddInvalidData(-1, "New Tree.");
			OutputMessageBinding();
			UpdateTreeNameLabel();
		}

		private void CorrectBTTreeDepth()
		{
			int depth = -1;

			foreach (BTNode node in _tree)
			{
				int depth_limit = depth + 1;
				if (node._depth > depth_limit)
					node._depth = depth_limit;

				depth = node._depth;
			}

		}

		// update BTTree based on _tree list
		private void UpdateBTTree()
		{
			BTTree.Items.Clear();

			// debug tree info
			//Console.WriteLine("=================================================================");
			//foreach (BTNode item in _tree)
			//{
			//	for (int i = 0; i < item._depth; ++i)
			//	{
			//		Console.Write('\t');
			//	}
			//	Console.WriteLine("Name: " + item._name + ", depth: " + item._depth);

			//}

			Stack<TreeItem> treestack = new Stack<TreeItem>();
			TreeItem treeitem = new TreeItem();

			foreach (BTNode node in _tree)
			{
				TreeItem tmp_treeitem = new TreeItem(node);

				if (treestack.Count == 0)
				{
					treeitem = tmp_treeitem;
					treestack.Push(treeitem);
					BTTree.Items.Add(treeitem);
				}
				else
				{
					while (treestack.Peek()._node._depth >= node._depth)
					{
						treestack.Pop();
					}

					if (treestack.Peek()._node._depth < node._depth)
					{
						treeitem = tmp_treeitem;
						treestack.Peek().Items.Add(treeitem);
						treestack.Push(treeitem);
					}
				}
			}

			BTButtons btbuttons = UIHelper.FindChild<BTButtons>(App._wnd, "MyBTButtons");
			btbuttons.CheckButtonEnabled();

		}

		private bool FindTreeViewItemFromBTTree(int index, out TreeViewItem item)
		{
			if (BTTree == null)
			{
				item = null;
				return false;
			}
			else if (index == 0)
			{
				item = BTTree.ItemContainerGenerator.ContainerFromIndex(0) as TreeViewItem;
				return true;
			}
			else
			{
				TreeViewItem treeviewitem = BTTree.ItemContainerGenerator.ContainerFromIndex(0) as TreeViewItem;

				return FindTreeViewItemFromNode(treeviewitem, index, out item);
			}
		}

		private bool FindTreeViewItemFromNode(TreeViewItem basenode, int index, out TreeViewItem item)
		{
			for (int i = 0; i < basenode.Items.Count; ++i)
			{
				TreeViewItem treeviewitem = basenode.ItemContainerGenerator.ContainerFromIndex(i) as TreeViewItem;

				if (treeviewitem == null)
					continue;

				TreeItem treeitem = basenode.Items[i] as TreeItem;

				if (treeitem._node._index == index)
				{
					item = treeviewitem;

					return true;
				}

				if (FindTreeViewItemFromNode(treeviewitem, index, out item) == true)
					return true;
			}

			item = null;

			return false;
		}

		// save _tree to file
		public void SaveTreeToFile(string filename)
		{
			using (System.IO.StreamWriter outfile = new System.IO.StreamWriter(filename))
			{
				// save treename
				outfile.WriteLine("TREENAME(" + App._global._treeName + ")");

				foreach (BTNode node in _tree)
				{
					outfile.WriteLine("TREENODE(" + node._name + ", " + node._depth + ")");
				}
			}

			_invalid_data.Clear();
			AddInvalidData(-1, "File " + App._global._treeName + ".bht Saved.");
			OutputMessageBinding();
			UpdateTreeNameLabel();
		}

		// load file to _tree
		public void LoadTreeFromFile(string filename)
		{
			_tree.Clear();

			using (System.IO.StreamReader infile = new System.IO.StreamReader(filename))
			{
				string line;

				// get tree name
				line = infile.ReadLine();
				{
					string[] seperators = { "TREENAME(", ")" };
					var parts = line.Split(seperators, StringSplitOptions.RemoveEmptyEntries);
					App._global._treeName = parts[0];
				}

				while ((line = infile.ReadLine()) != null)
				{
					// string format is "TREENODE(node_name, node_depth)"
					string[] seperators = { "TREENODE(", ",", ")" };
					var parts = line.Split(seperators, StringSplitOptions.RemoveEmptyEntries);
					BTNode node = new BTNode();

					// assign member variables
					node._name = parts[0];
					node._depth = Int32.Parse(parts[1]);
					BTNode nodedata = FindNodeData(node._name);
					node._summary = nodedata._summary;
					node._type = nodedata._type;

					_tree.Add((BTNode)node.Clone());
				}
			}

			UpdateTreeIndex();
			UpdateBTTree();

			_invalid_data.Clear();
			AddInvalidData(-1, "File " + App._global._treeName + ".bht Loaded.");
			OutputMessageBinding();
			UpdateTreeNameLabel();
		}

		// use node name to find BTNode data from _nodes list
		private BTNode FindNodeData(string nodename)
		{
			foreach (BTNode node in _nodes)
			{
				if (node._name == nodename)
					return node;
			}

			return null;
		}

		// add invalid data to list
		private void AddInvalidData(int index, string summary)
		{
			TreeValidateData data = new TreeValidateData() { _index = index };
			if (index < 0)
				data._summary = summary;
			else
				data._summary = string.Format("(" + index + ") " + summary);

			_invalid_data.Add(data);
		}

		// change treeview item color for invalid tree node
		private void MarkInvalidTreeNode(bool isReset)
		{
			if (_invalid_index < 0)
				return;

			foreach (TreeValidateData data in _invalid_data)
			{
				if (_invalid_data.Count <= _invalid_index)
					return;

				if (data._index == _invalid_data[_invalid_index]._index)
				{
					TreeViewItem treeviewitem = null;
					FindTreeViewItemFromBTTree(data._index, out treeviewitem);

					if (treeviewitem != null)
					{
						if (isReset)
							treeviewitem.Background = Brushes.White;
						else
							treeviewitem.Background = Brushes.Red;
					}
				}
			}
		}

		// check if tree is valid
		public bool ValidateTree()
		{
			_invalid_data.Clear();

			// tree can't be empty
			if (App._global._treeName == null)
			{
				AddInvalidData(-1, "Tree Name Is Empty!");

				return false;
			}

			// tree can't be empty
			if (_tree.Count == 0)
			{
				AddInvalidData(0, "Tree Can't Be Empty!");

				return false;
			}

			// root node must have depth 0
			if (_tree[0]._depth != 0)
			{
				AddInvalidData(0, "Depth of Root Node Has To Be Zero!");

				return false;
			}

			// root node must be composite node or decorate node
			if ((_tree[0]._type != BTNodeType.Composite) && (_tree[0]._type != BTNodeType.Decorator))
			{
				AddInvalidData(0, "Root Node Must Be Composite Or Decorator Type!");
			}

			ValidateTreeNode((TreeItem)BTTree.Items[0]);

			if (_invalid_data.Count == 0)
			{
				AddInvalidData(-1, "Valid Tree.");

				return true;
			}

			//MarkInvalidTreeNode();

			return false;
		}

		// check if tree node is valid
		private void ValidateTreeNode(TreeItem item)
		{
			switch(item._node._type)
			{
				case BTNodeType.Composite:
					// decorator can only have one child
					if (item.Items.Count < 2)
					{
						AddInvalidData(item._node._index, "Composite Node Must Have Multiple Children!");
					}
					break;

				case BTNodeType.Decorator:
					// decorator can only have one child
					if (item.Items.Count != 1)
					{
						AddInvalidData(item._node._index, "Decorator Node Must Have One Child!");
					}
					break;

				case BTNodeType.Leaf:
					{
						// leaf node can't have child
						if (item.Items.Count > 0)
						{
							AddInvalidData(item._node._index, "Leaf Node Can't Have Child!");
						}
					}
					break;

				default:
				case BTNodeType.None:
					break;
			}

			foreach (TreeItem treeitem in item.Items)
			{
				ValidateTreeNode(treeitem);
			}

		}

		private void BTNodes_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			App._global._hasDataInSelectedNode = false;
			App._global._selected_nodeindex = -1;

			BTNodes.UnselectAll();
		}

		private void BTNodes_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			if (App._global._previousMouseLeftButtonDown == true)
				return;

			if (BTNodes.SelectedIndex >= 0)
			{
				App._global._selectedNode = _nodes[BTNodes.SelectedIndex];
				App._global._selected_nodeindex = BTNodes.SelectedIndex;
				App._global._hasDataInSelectedNode = true;
				//Console.WriteLine("(S)selected : {0}", App.Global._selectedNode._name);

				App._global._previousMouseLeftButtonDown = true;
			}
		}

		private void BTNodes_PreviewMouseMove(object sender, MouseEventArgs e)
		{
			// Get the current mouse position
			Point mousePos = e.GetPosition(null);
			Vector diff = startPoint - mousePos;

			if (e.LeftButton == MouseButtonState.Pressed &&
				(Math.Abs(diff.X) > SystemParameters.MinimumHorizontalDragDistance ||
				Math.Abs(diff.Y) > SystemParameters.MinimumVerticalDragDistance))
			{
				// Get the dragged ListViewItem
				ListBox listBox = sender as ListBox;
				ListBoxItem listBoxItem =
					FindAnchestor<ListBoxItem>((DependencyObject)e.OriginalSource);

				if (listBoxItem == null)
					return;

				// Find the data behind the ListViewItem
				BTNode node = (BTNode)listBox.ItemContainerGenerator.
					ItemFromContainer(listBoxItem);

				// Initialize the drag & drop operation
				DataObject dragData = new DataObject("BehaviorNode", node);
				DragDrop.DoDragDrop(listBoxItem, dragData, DragDropEffects.Move);
			}
		}

		private void BTNodes_Loaded(object sender, RoutedEventArgs e)
		{
			//LoadNodes();
		}

		private void BTTree_DragEnter(object sender, DragEventArgs e)
		{
			if (!e.Data.GetDataPresent("BehaviorNode") || sender == e.Source)
			{
				e.Effects = DragDropEffects.None;
			}

		}

		private void BTTree_Drop(object sender, DragEventArgs e)
		{
			if (e.Data.GetDataPresent("BehaviorNode"))
			{
				BTNode node = e.Data.GetData("BehaviorNode") as BTNode;

				// Get the dragged ListViewItem
				TreeView treeView = sender as TreeView;

				// Get the dragged ListViewItem
				TreeViewItem treeViewItem =
					FindAnchestor<TreeViewItem>((DependencyObject)e.OriginalSource);

				// drop item into new place, add it to the end of tree
				if (treeViewItem == null)
				{
					AddNodeToTree((BTNode)node.Clone());
				}
				else
				{
					// Find the data behind the TreeViewItem
					TreeItem dropspot_node = (TreeItem)treeViewItem.Header;
					//TreeItem dropspot_node = (TreeItem)BTTree.ItemContainerGenerator.
					//    ItemFromContainer(treeViewItem);

					InsertNodeToTree((BTNode)node.Clone(), dropspot_node._node._index);
				}

				// build BTTree

				UpdateBTTree();

				//foreach (BTNode btnode in _tree)
				//{
				//    TreeItem treeitem = new TreeItem { _node = btnode };

				//    BTTree.Items.Add(treeitem);
				//}
			}
		}

		private void BTTree_DragOver(object sender, DragEventArgs e)
		{
			TreeViewItem treeViewItem = FindAnchestor<TreeViewItem>((DependencyObject)e.OriginalSource);

			if (treeViewItem != null)
			{
				treeViewItem.Background = Brushes.Yellow;
			}
		}

		private void BTTree_DragLeave(object sender, DragEventArgs e)
		{
			TreeViewItem treeViewItem = FindAnchestor<TreeViewItem>((DependencyObject)e.OriginalSource);

			if (treeViewItem != null)
			{
				treeViewItem.Background = Brushes.White;
			}
		}

		private void OutputMessage_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			ListBox listbox = sender as ListBox;

			MarkInvalidTreeNode(true);
			_invalid_index = OutputMessage.SelectedIndex;
			MarkInvalidTreeNode(false);
		}

		private void BTTree_PreviewKeyDown(object sender, KeyEventArgs e)
		{
			// right key to add depth
			if (e.Key == Key.Right)
			{
				TreeItem item = (TreeItem)BTTree.SelectedItem;

				if (item == null)
					return;

				int selected_index = item._node._index;

				if (selected_index != 0)
				{
					int upperlimit = _tree[selected_index - 1]._depth + 1;

					_tree[selected_index]._depth += 1;
					if (_tree[selected_index]._depth > upperlimit)
						_tree[selected_index]._depth = upperlimit;

					UpdateBTTree();

					// get selecteditem
					UpdateLayout();		// need to call this to ensures that all visual child elements of this 
										// element are properly updated for layout
					TreeViewItem treeviewitem;
					if (FindTreeViewItemFromBTTree(selected_index, out treeviewitem))
						treeviewitem.IsSelected = true;
				}
			}
			// left key to sub depth
			else if (e.Key == Key.Left)
			{
				TreeItem item = (TreeItem)BTTree.SelectedItem;
				if (item == null)
					return;

				int selected_index = item._node._index;

				if (selected_index != 0)
				{
					_tree[selected_index]._depth -= 1;
					if (_tree[selected_index]._depth < 1)
						_tree[selected_index]._depth = 1;

					CorrectBTTreeDepth();
					UpdateBTTree();

					// get selecteditem
					UpdateLayout();		// need to call this to ensures that all visual child elements of this 
										// element are properly updated for layout
					TreeViewItem treeviewitem;
					if (FindTreeViewItemFromBTTree(selected_index, out treeviewitem))
						treeviewitem.IsSelected = true;
				}
			}
			// up key to swap node with previous one
			else if (e.Key == Key.Up)
			{
				TreeItem item = (TreeItem)BTTree.SelectedItem;
				if (item == null)
					return;

				int selected_index = item._node._index;

				if (selected_index != 0)
				{
					int previous_index = selected_index - 1;
					int current_depth = _tree[selected_index]._depth;
					int previous_depth = _tree[previous_index]._depth;

					_tree.Swap(selected_index, previous_index);
					_tree[previous_index]._depth = previous_depth;
					_tree[previous_index]._index = previous_index;
					_tree[selected_index]._depth = current_depth;
					_tree[selected_index]._index = selected_index;

					CorrectBTTreeDepth();
					UpdateBTTree();

					// get selecteditem
					UpdateLayout();		// need to call this to ensures that all visual child elements of this 
					// element are properly updated for layout
					TreeViewItem treeviewitem;
					if (FindTreeViewItemFromBTTree(previous_index, out treeviewitem))
						treeviewitem.IsSelected = true;
				}
			}
			// down key to swap node with previous one
			else if (e.Key == Key.Down)
			{
				TreeItem item = (TreeItem)BTTree.SelectedItem;
				if (item == null)
					return;

				int selected_index = item._node._index;

				if (selected_index != _tree.Count - 1)
				{
					int next_index = selected_index + 1;
					int current_depth = _tree[selected_index]._depth;
					int next_depth = _tree[next_index]._depth;

					_tree.Swap(selected_index, next_index);
					_tree[next_index]._depth = next_depth;
					_tree[next_index]._index = next_index;
					_tree[selected_index]._depth = current_depth;
					_tree[selected_index]._index = selected_index;

					CorrectBTTreeDepth();
					UpdateBTTree();

					// get selecteditem
					UpdateLayout();		// need to call this to ensures that all visual child elements of this 
					// element are properly updated for layout
					TreeViewItem treeviewitem;
					if (FindTreeViewItemFromBTTree(next_index, out treeviewitem))
						treeviewitem.IsSelected = true;
				}
			}
			// delete to remove node
			else if (e.Key == Key.Delete)
			{
				TreeItem item = (TreeItem)BTTree.SelectedItem;
				if (item == null)
					return;

				int selected_index = item._node._index;

				RemoveNodeFromTree(selected_index);
				CorrectBTTreeDepth();
				UpdateBTTree();

				if (_tree.Count != 0)
				{
					// get selecteditem
					UpdateLayout();		// need to call this to ensures that all visual child elements of this 
										// element are properly updated for layout
					TreeViewItem treeviewitem;

					if (FindTreeViewItemFromBTTree(selected_index, out treeviewitem))
						treeviewitem.IsSelected = true;
				}
			}

			e.Handled = true;	// make sure keystrokes only be handled once
		}
	}

	///////////////////////////////////////////////////////////////////////////

	// Behavior Tree node type
	public enum BTNodeType
	{
		None,
		Composite,
		Decorator,
		Leaf,
	}

	// Behavior Tree node node info
	[Serializable]
	public class BTNode : ICloneable
	{
		// name of the node
		public string _name { get; set; }
		// summary of the node
		public string _summary { get; set; }
		// type of the node
		public BTNodeType _type { get; set; }
		// index of the node on nodelist
		public int _index { get; set; }
		// depth of the node on tree
		public int _depth { get; set; }

		public object Clone()
		{
			return MemberwiseClone();
		}
	}

	// treeview items
	public class TreeItem : TreeView
	{
		public TreeItem()
		{
			this.Items = new ObservableCollection<TreeItem>();
		}

		public TreeItem(BTNode node)
		{
			this.Items = new ObservableCollection<TreeItem>();
			_node = node;
			_printline = string.Format("({0}) {1}", _node._index, _node._name);
		}

		public BTNode _node { get; set; }
		public int _treeindex { get; set; }
		public string _printline { get; set; }

		public new ObservableCollection<TreeItem> Items { get; set; }
	}

	// tree validate data
	public class TreeValidateData
	{
		public int _index { get; set; }
		public int _treeindex { get; set; }
		public string _summary { get; set; }
	}

}
