using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

using RateRacerCore;

namespace RateRacerGUI
{
	/// <summary>
	/// Summary description for MainForm.
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
    //private Gweronimo.BmpControl bmpControl1;
    //private Gweronimo.GLPreviewControl glPreviewControl1;

    private Gweronimo.BmpControl bmpControl1;
    private Gweronimo.GLPreviewControl glPreviewControl1;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Panel panel2;
    private System.Windows.Forms.Button button1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public MainForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//

      this.bmpControl1 = new Gweronimo.BmpControl();
      this.glPreviewControl1 = new Gweronimo.GLPreviewControl();
      // 
      // bmpControl1
      // 
      this.bmpControl1.Name = "bmpControl1";
      this.bmpControl1.Text = "bmpControl1";
      this.bmpControl1.Location = new System.Drawing.Point(24, 32);
      this.bmpControl1.Size = new System.Drawing.Size(200, 200);
      this.bmpControl1.Dock = DockStyle.Fill;
      this.bmpControl1.TabIndex = 0;
      // 
      // glPreviewControl1
      // 
      this.glPreviewControl1.Name = "glPreviewControl1";
      this.glPreviewControl1.Text = "glPreviewControl1";
      this.glPreviewControl1.Location = new System.Drawing.Point(232, 40);
      this.glPreviewControl1.Size = new System.Drawing.Size(320, 176);
      this.glPreviewControl1.Dock = DockStyle.Fill;
      this.glPreviewControl1.TabIndex = 1;

      this.SuspendLayout();
      panel1.Controls.Add(this.glPreviewControl1);
      panel2.Controls.Add(this.bmpControl1);
      this.ResumeLayout(false);

      bmpControl1.createBitmap();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MainForm));
      this.panel1 = new System.Windows.Forms.Panel();
      this.panel2 = new System.Windows.Forms.Panel();
      this.button1 = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // panel1
      // 
      this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.panel1.Location = new System.Drawing.Point(8, 8);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(288, 400);
      this.panel1.TabIndex = 0;
      // 
      // panel2
      // 
      this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.panel2.Location = new System.Drawing.Point(328, 8);
      this.panel2.Name = "panel2";
      this.panel2.Size = new System.Drawing.Size(400, 400);
      this.panel2.TabIndex = 1;
      // 
      // button1
      // 
      this.button1.Location = new System.Drawing.Point(304, 114);
      this.button1.Name = "button1";
      this.button1.Size = new System.Drawing.Size(16, 95);
      this.button1.TabIndex = 2;
      this.button1.Text = "Render";
      this.button1.Click += new System.EventHandler(this.button1_Click);
      // 
      // MainForm
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(736, 418);
      this.Controls.Add(this.button1);
      this.Controls.Add(this.panel2);
      this.Controls.Add(this.panel1);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "MainForm";
      this.Text = "Rate Racer - a renderer";
      this.ResumeLayout(false);

    }
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
      RateRacerEngine.init();
			Application.Run(new MainForm());
      RateRacerEngine.finish();
    }

    private void MainForm_Load(object sender, System.EventArgs e)
    {
    }

    private void button1_Click(object sender, System.EventArgs e)
    {
      RateRacerEngine.render();
    }
	}
}
