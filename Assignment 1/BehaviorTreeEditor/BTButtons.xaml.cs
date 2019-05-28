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
using Microsoft.Win32;

namespace BehaviorTreeEditor
{
	/// <summary>
	/// Interaction logic for BTButtons.xaml
	/// </summary>
	public partial class BTButtons : UserControl
	{
		public BTButtons()
		{
			InitializeComponent();

		}

		public void CheckButtonEnabled()
		{
			if (App._global._treeName == null || App._global._treeName == "")
			{
				SaveButton.IsEnabled = false;
				App._wnd.MenuSave.IsEnabled = false;
			}
			else
			{
				SaveButton.IsEnabled = true;
				App._wnd.MenuSave.IsEnabled = true;
			}

			TreeEdit treeedit = UIHelper.FindChild<TreeEdit>(App._wnd, "MyTreeEdit");
			if (treeedit._tree.Count == 0)
			{
				SaveButton.IsEnabled = false;
				App._wnd.MenuSave.IsEnabled = false;
				SaveAsButton.IsEnabled = false;
				App._wnd.MenuSaveAs.IsEnabled = false;
			}
			else
			{
				SaveAsButton.IsEnabled = true;
				App._wnd.MenuSaveAs.IsEnabled = true;
			}
			
		}

		// do confirmation
		public bool Confirmation(string messageBoxText)
		{
			string caption = "Confirmation";
			MessageBoxButton button = MessageBoxButton.OKCancel;
			MessageBoxImage icon = MessageBoxImage.Question;
			// Display message box
			var result = MessageBox.Show(messageBoxText, caption, button, icon);

			if (result == MessageBoxResult.OK)
				return true;

			return false;
		}

		// new tree name
		public void NewTreeName()
		{
			InputDialog inputdialog = new InputDialog();
			inputdialog.InputText.Text = App._global._treeName;
			inputdialog.ShowDialog();

			CheckButtonEnabled();
		}

		// load tree file
		public void LoadFromFile()
		{
			TreeEdit treeedit = UIHelper.FindChild<TreeEdit>(App._wnd, "MyTreeEdit");

			OpenFileDialog openFileDialog = new OpenFileDialog();
			openFileDialog.Filter = "Behavior Tree files (*.bht)|*.bht";
			//openFileDialog.InitialDirectory = System.Reflection.Assembly.GetExecutingAssembly().Location;
			openFileDialog.InitialDirectory = App._projectDir + App._resourcesDir;

			if (openFileDialog.ShowDialog() == true)
			{
				// get tree name from file
				string treename = openFileDialog.SafeFileName;
				App._global._treeName = treename.Substring(0, treename.IndexOf('.'));

				treeedit.LoadTreeFromFile(openFileDialog.FileName);
			}
		}

		// save tree to file
		public void SaveToFile()
		{
			if (ValidateTree() == false)
				return;

			if (App._global._treeName != null)
			{
				TreeEdit treeedit = UIHelper.FindChild<TreeEdit>(App._wnd, "MyTreeEdit");
				string filename = App._projectDir + App._resourcesDir + App._global._treeName + ".bht";

				treeedit.SaveTreeToFile(filename);
			}

		}

		// save tree to new file
		public void SaveToNewFile()
		{
			if (ValidateTree() == false)
				return;

			TreeEdit treeedit = UIHelper.FindChild<TreeEdit>(App._wnd, "MyTreeEdit");

			SaveFileDialog saveFileDialog = new SaveFileDialog();
			saveFileDialog.OverwritePrompt = true;
			saveFileDialog.Filter = "Behavior Tree files (*.bht)|*.bht";
			//saveFileDialog.InitialDirectory = System.Reflection.Assembly.GetExecutingAssembly().Location + App._resourcesDir;
			saveFileDialog.InitialDirectory = App._projectDir + App._resourcesDir;

			if (saveFileDialog.ShowDialog() == true)
			{
				// get tree name from file
				string treename = saveFileDialog.SafeFileName;
				App._global._treeName = treename.Substring(0, treename.IndexOf('.'));

				treeedit.SaveTreeToFile(saveFileDialog.FileName);
			}
		}

		// validate tree
		public bool ValidateTree()
		{
			TreeEdit treeedit = UIHelper.FindChild<TreeEdit>(App._wnd, "MyTreeEdit");

			bool isvalid = treeedit.ValidateTree();
			treeedit.OutputMessageBinding();

			return isvalid;
		}

		public void NewButton_Click(object sender, RoutedEventArgs e)
		{
			TreeEdit treeedit = UIHelper.FindChild<TreeEdit>(App._wnd, "MyTreeEdit");

			if ((treeedit._tree.Count > 0) && (!Confirmation("Tree Not Empty.\nClear Old Tree?")))
				return;

			NewTreeName();

			treeedit.EmptyTree();
		}

		public void LoadButton_Click(object sender, RoutedEventArgs e)
		{
			TreeEdit treeedit = UIHelper.FindChild<TreeEdit>(App._wnd, "MyTreeEdit");

			if ((treeedit._tree.Count > 0) && (!Confirmation("Tree Not Empty.\nClear Old Tree?")))
				return;

			LoadFromFile();
		}

		public void ValidateButton_Click(object sender, RoutedEventArgs e)
		{
			ValidateTree();
		}

		public void SaveButton_Click(object sender, RoutedEventArgs e)
		{
			if (!Confirmation("Are You Sure?"))
				return;

			SaveToFile();
		}

		public void SaveAsButton_Click(object sender, RoutedEventArgs e)
		{
			SaveToNewFile();
		}

		public void RenameButton_Click(object sender, RoutedEventArgs e)
		{
			NewTreeName();
		}
	}
}
