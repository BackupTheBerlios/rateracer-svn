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
    private RenderForm mRenderForm;

    private Gweronimo.GLPreviewControl glPreviewControl1;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.MainMenu mainMenu1;
    private System.Windows.Forms.MenuItem menuItem1;
    private System.Windows.Forms.MenuItem menuItem2;
    private System.Windows.Forms.MenuItem miRender;
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

      this.glPreviewControl1 = new Gweronimo.GLPreviewControl();
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
      this.ResumeLayout(false);
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
      this.mainMenu1 = new System.Windows.Forms.MainMenu();
      this.menuItem1 = new System.Windows.Forms.MenuItem();
      this.menuItem2 = new System.Windows.Forms.MenuItem();
      this.miRender = new System.Windows.Forms.MenuItem();
      this.SuspendLayout();
      // 
      // panel1
      // 
      this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.panel1.Location = new System.Drawing.Point(8, 8);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(680, 352);
      this.panel1.TabIndex = 0;
      // 
      // mainMenu1
      // 
      this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                              this.menuItem1,
                                                                              this.miRender});
      // 
      // menuItem1
      // 
      this.menuItem1.Index = 0;
      this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                              this.menuItem2});
      this.menuItem1.Text = "Rendering";
      // 
      // menuItem2
      // 
      this.menuItem2.Index = 0;
      this.menuItem2.Text = "Use pathtracing";
      // 
      // miRender
      // 
      this.miRender.Index = 1;
      this.miRender.Text = "Render!";
      this.miRender.Click += new System.EventHandler(this.miRender_Click);
      // 
      // MainForm
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(696, 368);
      this.Controls.Add(this.panel1);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Menu = this.mainMenu1;
      this.Name = "MainForm";
      this.Text = "Rate Racer - a renderer";
      this.Load += new System.EventHandler(this.MainForm_Load);
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

      Application.EnableVisualStyles();
      Application.DoEvents();

      MainForm mainForm = new MainForm();
			Application.Run(mainForm);
      mainForm.Dispose();

      RateRacerEngine.finish();
    }

    private void MainForm_Load(object sender, System.EventArgs e)
    {
    
    }

    private void miRender_Click(object sender, System.EventArgs e)
    {
      startRender();
    }

    void startRender()
    {
      Console.WriteLine("startRender: " + System.Threading.Thread.CurrentThread.GetHashCode());
      if (mRenderForm == null) 
      {
        mRenderForm = new RenderForm();

        MethodInvoker mi = new MethodInvoker(createRenderWindow);
        mi.BeginInvoke(null, null);

        //mRenderForm.startRendering();
      }
      else {
        mRenderForm.startRendering();
      }
    }

    void createRenderWindow()
    {
      Console.WriteLine("create: " + System.Threading.Thread.CurrentThread.GetHashCode());
      mRenderForm.Closed += new System.EventHandler(this.RenderForm_Closed);
      mRenderForm.Show();

      Application.Run();
    }

    private void RenderForm_Closed(object sender, System.EventArgs e)
    {
      Console.WriteLine("Closed: " + System.Threading.Thread.CurrentThread.GetHashCode());
      mRenderForm = null;
    }

  }
}
