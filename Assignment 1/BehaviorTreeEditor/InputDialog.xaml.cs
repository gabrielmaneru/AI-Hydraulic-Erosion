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
using System.Windows.Shapes;

namespace BehaviorTreeEditor
{
    /// <summary>
    /// Interaction logic for InputDialog.xaml
    /// </summary>
    public partial class InputDialog : Window
    {
        public InputDialog()
        {
            InitializeComponent();

            Loaded += InputDialog_Loaded;
        }

        void InputDialog_Loaded(object sender, RoutedEventArgs e)
        {
        }

        private void OKButton_Click(object sender, RoutedEventArgs e)
        {
            if (InputText.Text != null && InputText.Text != "")
                App._global._treeName = InputText.Text;

            TreeEdit treeedit = UIHelper.FindChild<TreeEdit>(App._wnd, "MyTreeEdit");
            treeedit.UpdateTreeNameLabel();

            this.Close();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
