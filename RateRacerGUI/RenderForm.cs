using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using RateRacerCore;

namespace RateRacerGUI
{
	/// <summary>
	/// Summary description for RenderForm.
	/// </summary>
	public class RenderForm : System.Windows.Forms.Form
	{
    Size mRenderSize;
    float mZoom;

    private Gweronimo.BmpControl bmpControl1;
    private System.Windows.Forms.MainMenu mainMenu1;
    private System.Windows.Forms.MenuItem menuItem1;
    private System.Windows.Forms.MenuItem menuItem2;
    private System.Windows.Forms.MenuItem menuItem3;
    private System.Windows.Forms.MenuItem mi1x;
    private System.Windows.Forms.MenuItem mi2x;
    private System.Windows.Forms.MenuItem mi4x;
    private System.Windows.Forms.MenuItem mi8x;
    private System.Windows.Forms.Panel panel1;

    /// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public RenderForm(Size renderSize)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
      mRenderSize = renderSize;
      mZoom = 1;

      this.bmpControl1 = new Gweronimo.BmpControl();
      // 
      // bmpControl1
      // 
      this.bmpControl1.Name = "bmpControl1";
      this.bmpControl1.Text = "bmpControl1";
      this.bmpControl1.Location = new System.Drawing.Point(0, 0);
      this.bmpControl1.Size = mRenderSize;
      //this.bmpControl1.Anchor = AnchorStyles.Left | AnchorStyles.Top;
      //this.bmpControl1.Dock = DockStyle.Fill;
      this.bmpControl1.TabIndex = 0;

      panel1.Controls.Add(this.bmpControl1);

      //bmpControl1.createBitmap();
    }

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
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
      System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(RenderForm));
      this.mainMenu1 = new System.Windows.Forms.MainMenu();
      this.menuItem1 = new System.Windows.Forms.MenuItem();
      this.menuItem2 = new System.Windows.Forms.MenuItem();
      this.menuItem3 = new System.Windows.Forms.MenuItem();
      this.mi1x = new System.Windows.Forms.MenuItem();
      this.mi2x = new System.Windows.Forms.MenuItem();
      this.mi4x = new System.Windows.Forms.MenuItem();
      this.mi8x = new System.Windows.Forms.MenuItem();
      this.panel1 = new System.Windows.Forms.Panel();
      this.SuspendLayout();
      // 
      // mainMenu1
      // 
      this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                              this.menuItem1,
                                                                              this.menuItem3});
      // 
      // menuItem1
      // 
      this.menuItem1.Index = 0;
      this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                              this.menuItem2});
      this.menuItem1.Text = "Image";
      // 
      // menuItem2
      // 
      this.menuItem2.Index = 0;
      this.menuItem2.Text = "Save...";
      // 
      // menuItem3
      // 
      this.menuItem3.Index = 1;
      this.menuItem3.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                              this.mi1x,
                                                                              this.mi2x,
                                                                              this.mi4x,
                                                                              this.mi8x});
      this.menuItem3.Text = "Zoom";
      // 
      // mi1x
      // 
      this.mi1x.Index = 0;
      this.mi1x.Text = "1x";
      this.mi1x.Click += new System.EventHandler(this.menuZoom_Click);
      // 
      // mi2x
      // 
      this.mi2x.Index = 1;
      this.mi2x.Text = "2x";
      this.mi2x.Click += new System.EventHandler(this.menuZoom_Click);
      // 
      // mi4x
      // 
      this.mi4x.Index = 2;
      this.mi4x.Text = "4x";
      this.mi4x.Click += new System.EventHandler(this.menuZoom_Click);
      // 
      // mi8x
      // 
      this.mi8x.Index = 3;
      this.mi8x.Text = "8x";
      this.mi8x.Click += new System.EventHandler(this.menuZoom_Click);
      // 
      // panel1
      // 
      this.panel1.AutoScroll = true;
      this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel1.Location = new System.Drawing.Point(0, 0);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(292, 249);
      this.panel1.TabIndex = 0;
      // 
      // RenderForm
      // 
      this.AutoScale = false;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(292, 249);
      this.Controls.Add(this.panel1);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Menu = this.mainMenu1;
      this.Name = "RenderForm";
      this.Text = "RenderForm";
      this.Load += new System.EventHandler(this.RenderForm_Load);
      this.ResumeLayout(false);

    }
		#endregion

    void adjustSize(Size renderSize, float zoom)
    {
      if (renderSize != mRenderSize)
      {
        mRenderSize = renderSize;
        this.bmpControl1.setResolution( ref mRenderSize );
      }

      if (zoom != mZoom)
      {
        mZoom = zoom;
        this.bmpControl1.setZoom( mZoom );
      }

      Size zoomedSize = new Size(
        (int)(mZoom * (float)mRenderSize.Width),
        (int)(mZoom * (float)mRenderSize.Height));

      this.bmpControl1.Size = zoomedSize;

      this.ClientSize = zoomedSize;
      Size sizeDiff = this.panel1.Size - zoomedSize;
      if (sizeDiff.Width != 0 || sizeDiff.Height != 0)
      {
        Size formSize = this.Size + sizeDiff;
        //Screen.GetBounds(this);
        this.Size = formSize;
      }
    }

    delegate void setResolutionDelegate(Size renderSize);

    public void setResolution(Size renderSize)
    {
      if (this.InvokeRequired) 
      {
        //Console.WriteLine("invoke: " + System.Threading.Thread.CurrentThread.GetHashCode());
        setResolutionDelegate mi = new setResolutionDelegate(setResolution);
        this.BeginInvoke(mi, new object [] { renderSize } );
      }
      else 
      {
        //Console.WriteLine("setRes: " + System.Threading.Thread.CurrentThread.GetHashCode());
        //if (!Visible) Show();
        adjustSize(renderSize, mZoom);
      }
    }

    public void startRendering()
    {
      if (this.InvokeRequired) {
        //Console.WriteLine("invoke: " + System.Threading.Thread.CurrentThread.GetHashCode());
        MethodInvoker mi = new MethodInvoker(startRendering);
        this.BeginInvoke(mi);
      }
      else {
        //Console.WriteLine("render: " + System.Threading.Thread.CurrentThread.GetHashCode());
        if (!Visible) Show();
        Activate();
        RateRacerEngine.render();
      }
    }

    private void RenderForm_Load(object sender, System.EventArgs e)
    {
      //setResolution(mRenderSize);
      this.bmpControl1.setResolution( ref mRenderSize );
      this.bmpControl1.setZoom( mZoom );
      adjustSize(mRenderSize, mZoom);
      //startRendering();
      RateRacerEngine.render();
    }

    private void menuZoom_Click(object sender, System.EventArgs e)
    {
      float zoom = 1;

      if (sender.Equals(mi1x))
      {
        zoom = 1;
      }
      else if (sender.Equals(mi2x))
      {
        zoom = 2;
      }
      else if (sender.Equals(mi4x))
      {
        zoom = 4;
      }
      else if (sender.Equals(mi8x))
      {
        zoom = 8;
      }

      adjustSize(mRenderSize, zoom);
    }
	}
}
