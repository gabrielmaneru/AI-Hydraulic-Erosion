using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

using System.ComponentModel;
using System.IO;

namespace BehaviorTreeEditor
{
	/// <summary>
	/// Interaction logic for App.xaml
	/// </summary>
	public partial class App : Application
	{
		public static MainWindow _wnd;
		public static Global _global;

		//public static string _projectDir = Directory.GetCurrentDirectory();
		public static string _projectDir = System.Environment.CurrentDirectory;
		public static string _resourcesDir = "\\BTResources\\";
		public static string _treesFileName = "Trees.def";
		public static string _nodesFileName = "Nodes.def";

		// global variables

		public class Global
		{
			// BTTree name
			public string _treeName;
			// flag: if mouse is dragging
			public bool _isDragged;
			// flag: if there's data in selectedNode
			public bool _hasDataInSelectedNode;
			// flag: mouse left button previous status
			public bool _previousMouseLeftButtonDown;
			// node object being selected
			public BTNode _selectedNode;
			public int _selected_nodeindex;
		}

		private void Application_DispatcherUnhandledException(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e)
		{
			MessageBox.Show("An unhandeled exception just occurred: " + e.Exception.Message, "Exception", MessageBoxButton.OK, MessageBoxImage.Warning);
			e.Handled = true;
		}

		private void Application_Startup(object sender, StartupEventArgs e)
		{
			Initialize();

			// Create the startup window
			_wnd = new MainWindow();
			// Show the window
			_wnd.Show();
		}

		// Initialize global variables
		private void Initialize()
		{
			_global = new Global();

			_global._isDragged = false;
			_global._hasDataInSelectedNode = false;
			_global._previousMouseLeftButtonDown = false;
			_global._selected_nodeindex = -1;

		}
	}

	public static class UIHelper
	{
		/// <summary>
		/// Finds a Child of a given item in the visual tree. 
		/// </summary>
		/// <param name="parent">A direct parent of the queried item.</param>
		/// <typeparam name="T">The type of the queried item.</typeparam>
		/// <param name="childName">x:Name or Name of child. </param>
		/// <returns>The first parent item that matches the submitted type parameter. 
		/// If not matching item can be found, 
		/// a null parent is being returned.</returns>
		public static T FindChild<T>(DependencyObject parent, string childName)
		   where T : DependencyObject
		{
			// Confirm parent and childName are valid. 
			if (parent == null) return null;

			T foundChild = null;

			int childrenCount = VisualTreeHelper.GetChildrenCount(parent);
			for (int i = 0; i < childrenCount; i++)
			{
				var child = VisualTreeHelper.GetChild(parent, i);
				// If the child is not of the request child type child
				T childType = child as T;
				if (childType == null)
				{
					// recursively drill down the tree
					foundChild = FindChild<T>(child, childName);

					// If the child is found, break so we do not overwrite the found child. 
					if (foundChild != null) break;
				}
				else if (!string.IsNullOrEmpty(childName))
				{
					var frameworkElement = child as FrameworkElement;
					// If the child's name is set for search
					if (frameworkElement != null && frameworkElement.Name == childName)
					{
						// if the child's name is of the request name
						foundChild = (T)child;
						break;
					}
				}
				else
				{
					// child element found.
					foundChild = (T)child;
					break;
				}
			}

			return foundChild;
		}

		// swap method
		public static IList<T> Swap<T>(this IList<T> list, int indexA, int indexB)
		{
			if (indexB > -1 && indexB < list.Count)
			{
				T tmp = list[indexA];
				list[indexA] = list[indexB];
				list[indexB] = tmp;
			}
			return list;
		}
	}

}
