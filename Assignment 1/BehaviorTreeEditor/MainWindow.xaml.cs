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
using System.IO;

namespace BehaviorTreeEditor
{

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{

		///////////////////////////////////////////////////////////////////////

		// variables

		// mouse start position when dragging
		public Point _mouseStartPoint;

		public MainWindow()
		{
			InitializeComponent();
		}

		// Event: left mouse button pressed
		private void Grid_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			// store the mouse position
			_mouseStartPoint = e.GetPosition(null);
		}

		private void Grid_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			App._global._previousMouseLeftButtonDown = false;
		}

		// Event: mouse move
		private void Grid_PreviewMouseMove(object sender, MouseEventArgs e)
		{
			// get the current mouse position
			Point mouse_pos = e.GetPosition(null);
			Vector diff = _mouseStartPoint - mouse_pos;

			if (e.LeftButton == MouseButtonState.Pressed)
			{
				// check if drag distance is bigger than some small pixels
				// to prevent occasionally drags
				if ((Math.Abs(diff.X) > SystemParameters.MinimumHorizontalDragDistance) ||
				(Math.Abs(diff.Y) > SystemParameters.MinimumVerticalDragDistance))
				{
					if ((App._global._hasDataInSelectedNode == true) && (App._global._selected_nodeindex >= 0))
					{
						// for popup window
						// set it off, change placement, then set it back on

						NodePopUp.IsOpen = false;

						NodePopUp.PlacementRectangle = new Rect(new Point(e.GetPosition(this).X, 
							e.GetPosition(this).Y), new Point(800, 600));
						NodeOnPopupName.Text = App._global._selectedNode._name;

						App._global._isDragged = true;
						NodePopUp.IsOpen = true;

					}
				}
				else
				{
					App._global._isDragged = false;
					NodePopUp.IsOpen = false;
				}
			}
			else
			{
				App._global._isDragged = false;
				NodePopUp.IsOpen = false;
			}
		}

		private void Window_KeyDown(object sender, KeyEventArgs e)
		{
			//// write to file
			//if (e.Key == Key.F5)
			//{
			//    BTButtons btbuttons = UIHelper.FindChild<BTButtons>(App._wnd, "MyBTButtons");

			//    btbuttons.SaveToNewFile();
			//}
			//// load from file
			//else if (e.Key == Key.F8)
			//{
			//    BTButtons btbuttons = UIHelper.FindChild<BTButtons>(App._wnd, "MyBTButtons");

			//    btbuttons.LoadFromFile();
			//}
			//// validate tree
			//else if (e.Key == Key.F4)
			//{
			//    BTButtons btbuttons = UIHelper.FindChild<BTButtons>(App._wnd, "MyBTButtons");

			//    btbuttons.ValidateTree();
			//}
		}

		private void MenuNew_Click(object sender, RoutedEventArgs e)
		{
			BTButtons btbuttons = UIHelper.FindChild<BTButtons>(App._wnd, "MyBTButtons");

			btbuttons.NewButton_Click(sender, e);
		}

		private void MenuOpen_Click(object sender, RoutedEventArgs e)
		{
			BTButtons btbuttons = UIHelper.FindChild<BTButtons>(App._wnd, "MyBTButtons");

			btbuttons.LoadButton_Click(sender, e);
		}

		private void MenuSave_Click(object sender, RoutedEventArgs e)
		{
			BTButtons btbuttons = UIHelper.FindChild<BTButtons>(App._wnd, "MyBTButtons");

			btbuttons.SaveButton_Click(sender, e);
		}

		private void MenuSaveAs_Click(object sender, RoutedEventArgs e)
		{
			BTButtons btbuttons = UIHelper.FindChild<BTButtons>(App._wnd, "MyBTButtons");

			btbuttons.SaveAsButton_Click(sender, e);
		}

		private void MenuExit_Click(object sender, RoutedEventArgs e)
		{
			this.Close();
		}

		private void Window_Closed(object sender, EventArgs e)
		{
			// save tree files to Trees.def
			string full_resource_dir = App._projectDir + App._resourcesDir;
			string treesfile = full_resource_dir + App._treesFileName;
			using (System.IO.StreamWriter outfile = new System.IO.StreamWriter(treesfile))
			{
				foreach (string file in Directory.EnumerateFiles(full_resource_dir, "*.bht"))
				{
					int file_start_pos = file.LastIndexOf('\\') + 1;
					outfile.WriteLine("#include \"../BTResources/" + file.Substring(file_start_pos) + "\"");
				}
			}
		}

		private void Window_Loaded(object sender, RoutedEventArgs e)
		{
			TreeEdit treeedit = UIHelper.FindChild<TreeEdit>(this, "MyTreeEdit");
			treeedit.UpdateTreeNameLabel();
		}

		private void MenuRename_Click(object sender, RoutedEventArgs e)
		{
			BTButtons btbuttons = UIHelper.FindChild<BTButtons>(App._wnd, "MyBTButtons");
			btbuttons.RenameButton_Click(sender, e);
		}

		private void MenuInstruction_Click(object sender, RoutedEventArgs e)
		{
			// Configure the message box to be displayed
			string messageBoxText =
				"(Add Node):\n" +
				"Drag node from \"Node List\" to \"Behavior Tree\"\n\n" +
				"(Change Node Depth):\n" +
				"When node is selected:\n" +
				"\tpress 'right arrow' key to increase its depth.\n" +
				"\tpress 'left arrow' key to decrease its depth.\n" +
				"\tpress 'up arrow' key to swap the node with the one above.\n" +
				"\tpress 'down arrow' key to swap the node with the one below.\n" +
				"\tpress 'del' key to remove the node.\n\n" +
				"(Check Tree):\n" +
				"Use Validate button.\n" +
				"You can select each line at Message box to highlight the node that's not valid.\n\n" +
				"(Save Tree):\n" +
				"Save or Save as... button.";
			string caption = "Behavior Tree Editor Instruction";
			MessageBoxButton button = MessageBoxButton.OK;

			// Display message box
			MessageBox.Show(messageBoxText, caption, button);
		}

		private void MenuAbout_Click(object sender, RoutedEventArgs e)
		{
			// Configure the message box to be displayed
			string messageBoxText =
				"(Author)\n" +
				"Chi-Hao Kuo\n" +
				"chihao.kuo@digipen.edu\n\n" +
				"(Version)\n" +
				"1.0";
			string caption = "About Behavior Tree Editor";
			MessageBoxButton button = MessageBoxButton.OK;
			MessageBoxImage icon = MessageBoxImage.Information;

			// Display message box
			MessageBox.Show(messageBoxText, caption, button, icon);
		}

		private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			BTButtons btbuttons = UIHelper.FindChild<BTButtons>(App._wnd, "MyBTButtons");
			if (!btbuttons.Confirmation("Do You Want To Close Editor?"))
				e.Cancel = true;
		}
	}

}
