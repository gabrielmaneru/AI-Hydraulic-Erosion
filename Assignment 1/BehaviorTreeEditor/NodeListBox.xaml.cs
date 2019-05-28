using System;
using System.Collections.Generic;
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
    /// Interaction logic for NodeListBox.xaml
    /// </summary>
    public partial class NodeListBox : UserControl
    {
        // variables

        public List<BTNode> _nodes;
        public int _selected_nodeindex;

        // methods 

        public NodeListBox()
        {
            InitializeComponent();

            InitializeMembers();
            LoadNodes();
            ListBoxBinding();
        }

        // Initialize all member variables
        private void InitializeMembers()
        {
            _nodes = new List<BTNode>();
            _selected_nodeindex = 0;
        }

        // Load node info
        private void LoadNodes()
        {
            string[] lines = System.IO.File.ReadAllLines(@"..\..\..\..\Project\Sources\Nodes.def");
            int index = 0;

            foreach (string line in lines)
            {
                // get node type
                BTNodeType type = BTNodeType.None;

                if (line.StartsWith("REGISTER_COMPOSITE"))
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

        // select/deselect nodes
        private void BTNodes_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (_selected_nodeindex < 0)
            {
                dynamic selected_node = BTNodes.SelectedItem as dynamic;

                _selected_nodeindex = selected_node._index;

                Console.WriteLine("selected : {0}", _nodes[_selected_nodeindex]._name);

                BehaviorTreeEditor.App._wnd._selectedNode = _nodes[_selected_nodeindex];
            }
        }

        private void BTNodes_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _selected_nodeindex = -1;
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
    public struct BTNode
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
    }

}
