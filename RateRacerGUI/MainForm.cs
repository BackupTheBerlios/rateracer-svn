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
    private Size mRenderSize;

    private Gweronimo.GLPreviewControl glPreviewControl1;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.MainMenu mainMenu1;
    private System.Windows.Forms.MenuItem menuItem1;
    private System.Windows.Forms.MenuItem menuItem2;
    private System.Windows.Forms.MenuItem miRender;
    private System.Windows.Forms.MenuItem menuItem3;
    private System.Windows.Forms.MenuItem mi800x600;
    private System.Windows.Forms.MenuItem mi1024x768;
    private System.Windows.Forms.MenuItem mi640x480;
    private System.Windows.Forms.MenuItem mi320x240;
    private System.Windows.Forms.MenuItem mi160x120;
    private System.Windows.Forms.MenuItem mi80x60;
    private System.Windows.Forms.MenuItem mi1280x1024;
    private System.Windows.Forms.MenuItem mi1600x1200;
    private System.Windows.Forms.MenuItem mi1920x1440;
    private System.Windows.Forms.MenuItem mi2048x1536;
    private System.Windows.Forms.MenuItem menuItem4;
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

      mRenderSize = new Size(160, 120);

      this.glPreviewControl1 = new Gweronimo.GLPreviewControl();
      // 
      // glPreviewControl1
      // 
      this.glPreviewControl1.Name = "glPreviewControl1";
      this.glPreviewControl1.Text = "glPreviewControl1";
      this.glPreviewControl1.Location = new System.Drawing.Point(232, 40);
      this.glPreviewControl1.Size = new System.Drawing.Size(320, 176);
      //this.glPreviewControl1.Dock = DockStyle.Fill;
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
      this.menuItem3 = new System.Windows.Forms.MenuItem();
      this.mi800x600 = new System.Windows.Forms.MenuItem();
      this.mi1024x768 = new System.Windows.Forms.MenuItem();
      this.mi640x480 = new System.Windows.Forms.MenuItem();
      this.mi320x240 = new System.Windows.Forms.MenuItem();
      this.mi160x120 = new System.Windows.Forms.MenuItem();
      this.mi80x60 = new System.Windows.Forms.MenuItem();
      this.mi1280x1024 = new System.Windows.Forms.MenuItem();
      this.mi1600x1200 = new System.Windows.Forms.MenuItem();
      this.mi1920x1440 = new System.Windows.Forms.MenuItem();
      this.mi2048x1536 = new System.Windows.Forms.MenuItem();
      this.menuItem4 = new System.Windows.Forms.MenuItem();
      this.SuspendLayout();
      // 
      // panel1
      // 
      this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel1.Location = new System.Drawing.Point(0, 0);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(696, 368);
      this.panel1.TabIndex = 0;
      this.panel1.Resize += new System.EventHandler(this.panel1_Resize);
      // 
      // mainMenu1
      // 
      this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                              this.menuItem1,
                                                                              this.menuItem3,
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
      this.miRender.Index = 2;
      this.miRender.Text = "Render!";
      this.miRender.Click += new System.EventHandler(this.miRender_Click);
      // 
      // menuItem3
      // 
      this.menuItem3.Index = 1;
      this.menuItem3.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                              this.mi80x60,
                                                                              this.mi160x120,
                                                                              this.mi320x240,
                                                                              this.mi640x480,
                                                                              this.mi800x600,
                                                                              this.mi1024x768,
                                                                              this.mi1280x1024,
                                                                              this.mi1600x1200,
                                                                              this.mi1920x1440,
                                                                              this.mi2048x1536,
                                                                              this.menuItem4});
      this.menuItem3.Text = "Resolution";
      // 
      // mi800x600
      // 
      this.mi800x600.Index = 4;
      this.mi800x600.Text = "800 x 600";
      this.mi800x600.Click += new System.EventHandler(this.menuResolution_Click);
      // 
      // mi1024x768
      // 
      this.mi1024x768.Index = 5;
      this.mi1024x768.Text = "1024 x 768";
      this.mi1024x768.Click += new System.EventHandler(this.menuResolution_Click);
      // 
      // mi640x480
      // 
      this.mi640x480.Index = 3;
      this.mi640x480.Text = "640 x 480";
      this.mi640x480.Click += new System.EventHandler(this.menuResolution_Click);
      // 
      // mi320x240
      // 
      this.mi320x240.Index = 2;
      this.mi320x240.Text = "320 x 240";
      this.mi320x240.Click += new System.EventHandler(this.menuResolution_Click);
      // 
      // mi160x120
      // 
      this.mi160x120.Index = 1;
      this.mi160x120.Text = "160 x 120";
      this.mi160x120.Click += new System.EventHandler(this.menuResolution_Click);
      // 
      // mi80x60
      // 
      this.mi80x60.Index = 0;
      this.mi80x60.Text = "80 x 60";
      this.mi80x60.Click += new System.EventHandler(this.menuResolution_Click);
      // 
      // mi1280x1024
      // 
      this.mi1280x1024.Index = 6;
      this.mi1280x1024.Text = "1280 x 1024";
      this.mi1280x1024.Click += new System.EventHandler(this.menuResolution_Click);
      // 
      // mi1600x1200
      // 
      this.mi1600x1200.Index = 7;
      this.mi1600x1200.Text = "1600 x 1200";
      this.mi1600x1200.Click += new System.EventHandler(this.menuResolution_Click);
      // 
      // mi1920x1440
      // 
      this.mi1920x1440.Index = 8;
      this.mi1920x1440.Text = "1920 x 1440";
      this.mi1920x1440.Click += new System.EventHandler(this.menuResolution_Click);
      // 
      // mi2048x1536
      // 
      this.mi2048x1536.Index = 9;
      this.mi2048x1536.Text = "2048 x 1536";
      this.mi2048x1536.Click += new System.EventHandler(this.menuResolution_Click);
      // 
      // menuItem4
      // 
      this.menuItem4.Index = 10;
      this.menuItem4.Text = "Other...";
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
      //adjustPreviewSize();
    }

    private void panel1_Resize(object sender, System.EventArgs e)
    {
      adjustPreviewSize();
    }

    void adjustPreviewSize()
    {
      int x = 16, y = 16;
      int w = (panel1.Width - 32), h = (panel1.Height - 32);

      float ratio = (float)mRenderSize.Width / (float)mRenderSize.Height;
      float rw = h * ratio, rh = w / ratio;
      if (rw > w) { rw = w; } else { rh = h; }

      float rx = x + 0.5f * (w - rw), ry = y + 0.5f * (h - rh);
      glPreviewControl1.SetBounds((int)rx, (int)ry, (int)rw, (int)rh);
    }

    private void miRender_Click(object sender, System.EventArgs e)
    {
      startRender();
    }

    void startRender()
    {
      if (showRenderWindow()) 
      {
        mRenderForm.startRendering();
      }
    }

    bool showRenderWindow()
    {
      //Console.WriteLine("startRender: " + System.Threading.Thread.CurrentThread.GetHashCode());
      if (mRenderForm == null) 
      {
        mRenderForm = new RenderForm(mRenderSize);

        MethodInvoker mi = new MethodInvoker(createRenderWindowThread);
        mi.BeginInvoke(null, null);

        //mRenderForm.startRendering();

        return false;
      }
      return true;
    }

    void createRenderWindowThread()
    {
      //Console.WriteLine("create: " + System.Threading.Thread.CurrentThread.GetHashCode());
      mRenderForm.Closed += new System.EventHandler(this.RenderForm_Closed);
      mRenderForm.Show();

      Application.Run();
    }

    private void RenderForm_Closed(object sender, System.EventArgs e)
    {
      //Console.WriteLine("Closed: " + System.Threading.Thread.CurrentThread.GetHashCode());
      mRenderForm = null;
    }

    private void menuResolution_Click(object sender, System.EventArgs e)
    {
      if (sender.Equals(mi80x60))
      {
        mRenderSize.Width  = 80;
        mRenderSize.Height = 60;
      }
      else if (sender.Equals(mi160x120))
      {
        mRenderSize.Width  = 160;
        mRenderSize.Height = 120;
      }
      else if (sender.Equals(mi320x240))
      {
        mRenderSize.Width  = 320;
        mRenderSize.Height = 240;
      }
      else if (sender.Equals(mi640x480))
      {
        mRenderSize.Width  = 640;
        mRenderSize.Height = 480;
      }
      else if (sender.Equals(mi1024x768))
      {
        mRenderSize.Width  = 1024;
        mRenderSize.Height = 768;
      }
      else if (sender.Equals(mi1280x1024))
      {
        mRenderSize.Width  = 1280;
        mRenderSize.Height = 1024;
      }
      else if (sender.Equals(mi1600x1200))
      {
        mRenderSize.Width  = 1600;
        mRenderSize.Height = 1200;
      }
      else if (sender.Equals(mi1920x1440))
      {
        mRenderSize.Width  = 1920;
        mRenderSize.Height = 1440;
      }
      else if (sender.Equals(mi2048x1536))
      {
        mRenderSize.Width  = 2048;
        mRenderSize.Height = 1536;
      }

      adjustPreviewSize();

      if (showRenderWindow()) 
      {
        mRenderForm.setResolution( mRenderSize );
      }
    }

  }
}
