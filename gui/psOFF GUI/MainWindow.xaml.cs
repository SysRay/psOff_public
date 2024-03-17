using System;
using System.IO;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;

namespace psOFF_GUI
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            string currentDir = System.IO.Directory.GetCurrentDirectory();
            if (!checkForEmulator(currentDir))
            {
                MessageBox.Show("Please install the Emulator and move it to the dictonary", "Missing Emulator");
            }

        }

        public void MainGrid_Drop(object sender, DragEventArgs e)
        {
            
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

                // Check if there is at least one file and if it has the ".bin" extension.
                if (files.Length > 0 && (IsFileExtensionValid(files[0], ".bin") || IsFileExtensionValid(files[0], ".elf")))
                {
                    Pfad.Text = files[0];
                    OpenWindow1(files[0]);
                }
                else
                {
                    MessageBox.Show("Please drop a valid .bin or .elf file.", "Invalid File Type");
                }
            }
        }

        private bool IsFileExtensionValid(string filePath, string extension)
        {
            return System.IO.Path.GetExtension(filePath).Equals(extension, StringComparison.OrdinalIgnoreCase);
        }

        public void OpenWindow1(string filePath)
        {
            Window1 window1 = new Window1(filePath);
            window1.Show();

            // Close the current window if needed
            this.Close();
        }
        private bool checkForEmulator(string currentDir)
        {
            string emuPath = System.IO.Path.Combine(currentDir, "emulator.exe");
            return File.Exists(emuPath);
        }
       
    }
}