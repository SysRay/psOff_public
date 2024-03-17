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
using System.IO;

namespace psOFF_GUI
{
    public partial class Window1 : Window
    {
        private string filePath;
        
        public Window1(string filePath)
        {
            InitializeComponent();
            // Set the filePath property
            this.filePath = filePath;

            // Call the method to set the image source
            SetImageSource();
        }

        private void SetImageSource()
        {
            testText.Text = "0";
            // Check if filePath is not empty
            if (!string.IsNullOrEmpty(filePath))
            {
                testText.Text = "1";
                try
                {
                    // Construct the path to the image dynamically based on the provided file path
                    string directoryPath = System.IO.Path.GetDirectoryName(filePath);
                    string imagePath = System.IO.Path.Combine(directoryPath, "sce_sys", "icon0.png");
                    testText.Text = imagePath; 
                    // Set the source of the Image
                    if (File.Exists(imagePath))
                    {
                        iconImage.Source = new BitmapImage(new Uri(imagePath));
                    }
                    else
                    {
                        // Handle case where image file does not exist
                        MessageBox.Show("Image file not found.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                }
                catch (Exception ex)
                {
                    testText.Text = "2";
                    // Handle any exceptions
                    MessageBox.Show($"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }
    }
}
