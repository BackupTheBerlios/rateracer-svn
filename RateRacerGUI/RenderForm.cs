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

    //Timers.Timer mTimer;
    System.Windows.Forms.Timer mTimer;

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
    private System.Windows.Forms.Panel statusPanel;
    private System.Windows.Forms.Label labelStatus;
    private System.Windows.Forms.MenuItem miInterpolate;
    private System.Windows.Forms.MenuItem menuItem5;

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
      mi1x.Checked = true;

      this.bmpControl1 = new Gweronimo.BmpControl();
      this.bmpControl1.Name = "bmpControl1";
      this.bmpControl1.Text = "bmpControl1";
      this.bmpControl1.Location = new System.Drawing.Point(0, 0);
      this.bmpControl1.Size = mRenderSize;
      //this.bmpControl1.Anchor = AnchorStyles.Left | AnchorStyles.Top;
      //this.bmpControl1.Dock = DockStyle.Fill;
      this.bmpControl1.TabIndex = 0;

      panel1.Controls.Add(this.bmpControl1);

      labelStatus.Text = "Ready";

      mTimer = new System.Windows.Forms.Timer();
      mTimer.Tick += new EventHandler(mTimer_Tick);
      mTimer.Interval = 1000;
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
      this.statusPanel = new System.Windows.Forms.Panel();
      this.labelStatus = new System.Windows.Forms.Label();
      this.miInterpolate = new System.Windows.Forms.MenuItem();
      this.menuItem5 = new System.Windows.Forms.MenuItem();
      this.statusPanel.SuspendLayout();
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
                                                                              this.mi8x,
                                                                              this.menuItem5,
                                                                              this.miInterpolate});
      this.menuItem3.Text = "Zoom";
      // 
      // mi1x
      // 
      this.mi1x.Index = 0;
      this.mi1x.RadioCheck = true;
      this.mi1x.Text = "1x";
      this.mi1x.Click += new System.EventHandler(this.menuZoom_Click);
      // 
      // mi2x
      // 
      this.mi2x.Index = 1;
      this.mi2x.RadioCheck = true;
      this.mi2x.Text = "2x";
      this.mi2x.Click += new System.EventHandler(this.menuZoom_Click);
      // 
      // mi4x
      // 
      this.mi4x.Index = 2;
      this.mi4x.RadioCheck = true;
      this.mi4x.Text = "4x";
      this.mi4x.Click += new System.EventHandler(this.menuZoom_Click);
      // 
      // mi8x
      // 
      this.mi8x.Index = 3;
      this.mi8x.RadioCheck = true;
      this.mi8x.Text = "8x";
      this.mi8x.Click += new System.EventHandler(this.menuZoom_Click);
      // 
      // panel1
      // 
      this.panel1.AutoScroll = true;
      this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel1.Location = new System.Drawing.Point(0, 0);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(320, 249);
      this.panel1.TabIndex = 0;
      // 
      // statusPanel
      // 
      this.statusPanel.Controls.Add(this.labelStatus);
      this.statusPanel.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.statusPanel.Location = new System.Drawing.Point(0, 249);
      this.statusPanel.Name = "statusPanel";
      this.statusPanel.Size = new System.Drawing.Size(320, 20);
      this.statusPanel.TabIndex = 1;
      // 
      // labelStatus
      // 
      this.labelStatus.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.labelStatus.Location = new System.Drawing.Point(8, 4);
      this.labelStatus.Name = "labelStatus";
      this.labelStatus.Size = new System.Drawing.Size(304, 16);
      this.labelStatus.TabIndex = 0;
      this.labelStatus.Text = "labelStatus";
      // 
      // miInterpolate
      // 
      this.miInterpolate.Index = 5;
      this.miInterpolate.Text = "Interpolate";
      this.miInterpolate.Click += new System.EventHandler(this.miInterpolate_Click);
      // 
      // menuItem5
      // 
      this.menuItem5.Index = 4;
      this.menuItem5.Text = "-";
      // 
      // RenderForm
      // 
      this.AutoScale = false;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(320, 269);
      this.Controls.Add(this.panel1);
      this.Controls.Add(this.statusPanel);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Menu = this.mainMenu1;
      this.Name = "RenderForm";
      this.Text = "RenderForm";
      this.Load += new System.EventHandler(this.RenderForm_Load);
      this.statusPanel.ResumeLayout(false);
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

      this.ClientSize = zoomedSize + new Size(0,statusPanel.Height);
      Size sizeDiff = this.panel1.Size - zoomedSize;
      if (sizeDiff.Width != 0 || sizeDiff.Height != 0)
      {
        Size formSize = this.Size + sizeDiff;
        //Screen.GetBounds(this);
        this.Size = formSize;
      }
    }

    delegate void startRenderingDelegate(Size renderSize);

    public void startRendering(Size renderSize)
    {
      if (this.InvokeRequired) {
        //Console.WriteLine("invoke: " + System.Threading.Thread.CurrentThread.GetHashCode());
        startRenderingDelegate mi = new startRenderingDelegate(startRendering);
        this.BeginInvoke(mi, new object [] { renderSize } );
      }
      else {
        mTimer.Stop();
        //Console.WriteLine("render: " + System.Threading.Thread.CurrentThread.GetHashCode());
        if (!Visible) Show();
        Activate();

        adjustSize(renderSize, mZoom);
        RateRacerEngine.render();
        mTimer.Start();
      }
    }

    private void RenderForm_Load(object sender, System.EventArgs e)
    {
      this.bmpControl1.setResolution( ref mRenderSize );
      this.bmpControl1.setZoom( mZoom );
      startRendering( mRenderSize );
    }

    private string timeString(float seconds)
    {
      string timeStr = "";
      if (seconds < 60)
      {
        timeStr += seconds.ToString("f02") + "s";
      }
      else
      {
        TimeSpan ts = TimeSpan.FromMilliseconds(seconds * 1000);
        if (ts.Hours > 0) timeStr += ts.Hours + "h ";
        if (ts.Minutes > 0) timeStr += ts.Minutes + "m ";
        //if (ts.Seconds > 0) 
          timeStr += ts.Seconds + "s ";
        if (ts.Hours == 0 && ts.Minutes == 0) timeStr += ts.Milliseconds + "ms";
      }
      return timeStr;
    }

    private void mTimer_Tick(object sender, System.EventArgs e)
    {
      this.bmpControl1.updateBitmap();
      int percentage = this.bmpControl1.renderingPercentage();
      float timeSecs = this.bmpControl1.renderingTimeSeconds();
      labelStatus.Text = "Rendering: " + percentage + "%" +
        " Elapsed time: " + timeString(timeSecs);
      Console.WriteLine(labelStatus.Text);
      if (percentage == 100)
      {
        mTimer.Stop();
        Console.WriteLine("Stop!");
      }
    }

    private void menuZoom_Click(object sender, System.EventArgs e)
    {
      float zoom = 1;

      if (sender == mi1x)
      {
        zoom = 1;
      }
      else if (sender == mi2x)
      {
        zoom = 2;
      }
      else if (sender == mi4x)
      {
        zoom = 4;
      }
      else if (sender == mi8x)
      {
        zoom = 8;
      }

      mi1x.Checked = false;
      mi2x.Checked = false;
      mi4x.Checked = false;
      mi8x.Checked = false;
      ((MenuItem)sender).Checked = true;

      adjustSize(mRenderSize, zoom);
    }

    private void miInterpolate_Click(object sender, System.EventArgs e)
    {
      miInterpolate.Checked = !miInterpolate.Checked;
      this.bmpControl1.setInterpolation( miInterpolate.Checked );
      this.bmpControl1.Invalidate();
    }
	}
}
