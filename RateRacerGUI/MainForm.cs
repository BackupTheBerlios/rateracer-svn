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
    Size mRenderSize;
    double mAspectRatio;
    float mZoom;

    bool isRendering = false;

    //Timers.Timer mTimer;
    System.Windows.Forms.Timer mTimer;

    private Gweronimo.GLPreviewControl glPreviewControl1;
    private Gweronimo.BmpControl bmpControl1;
    private System.Windows.Forms.MainMenu mainMenu1;
    private System.Windows.Forms.MenuItem menuItem1;
    private System.Windows.Forms.MenuItem menuItem2;
    private System.Windows.Forms.PropertyGrid propertyGrid1;
    private System.Windows.Forms.Label lblStatus;
    private System.Windows.Forms.Panel sidePanel;
    private System.Windows.Forms.Panel previewPanel;
    private System.Windows.Forms.Panel statusPanel;
    private System.Windows.Forms.Panel renderPanel;
    private System.Windows.Forms.Panel propsPanel;
    private System.Windows.Forms.CheckBox chkKeepAspect;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.ComboBox cbProps;
    private System.Windows.Forms.ComboBox cbWidth;
    private System.Windows.Forms.ComboBox cbHeight;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.ComboBox cbZoom;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.CheckBox chkInterpolate;
    private System.Windows.Forms.Button btnRender;
    private System.Windows.Forms.CheckBox chkAutoRender;
    private System.Windows.Forms.Splitter splitter1;
    private System.Windows.Forms.Splitter splitter2;
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

      chkKeepAspect.Checked = true;

      mRenderSize = new Size(160, 120);
      mAspectRatio = (double)mRenderSize.Width / (double)mRenderSize.Height;
      cbWidth.Text = mRenderSize.Width.ToString();
      cbHeight.Text = mRenderSize.Height.ToString();
      cbWidth.Items.Add( 160); cbHeight.Items.Add( 120);
      cbWidth.Items.Add( 320); cbHeight.Items.Add( 240);
      cbWidth.Items.Add( 640); cbHeight.Items.Add( 480);
      cbWidth.Items.Add( 800); cbHeight.Items.Add( 600);
      cbWidth.Items.Add(1024); cbHeight.Items.Add( 768);
      cbWidth.Items.Add(1280); cbHeight.Items.Add( 960);

      chkInterpolate.Checked = false;

      mZoom = 1;
      cbZoom.Text = ((int)(mZoom*100)).ToString();
      cbZoom.Items.Add( 10);
      cbZoom.Items.Add( 25);
      cbZoom.Items.Add( 50);
      cbZoom.Items.Add( 75);
      cbZoom.Items.Add(100);
      cbZoom.Items.Add(200);
      cbZoom.Items.Add(300);
      cbZoom.Items.Add(400);
      cbZoom.Items.Add(500);
      cbZoom.Items.Add(600);
      cbZoom.Items.Add(700);
      cbZoom.Items.Add(800);

      lblStatus.Text = "Ready";

      mTimer = new System.Windows.Forms.Timer();
      mTimer.Tick += new EventHandler(mTimer_Tick);
      mTimer.Interval = 1000;

      glPreviewControl1 = new Gweronimo.GLPreviewControl();
      glPreviewControl1.Name = "glPreviewControl1";
      glPreviewControl1.Text = "glPreviewControl1";
      glPreviewControl1.Location = new System.Drawing.Point(232, 40);
      glPreviewControl1.Size = new System.Drawing.Size(320, 176);
      //glPreviewControl1.Dock = DockStyle.Fill;
      glPreviewControl1.TabIndex = 1;

      glPreviewControl1.KeyPress += new KeyPressEventHandler(image_KeyPress);
      glPreviewControl1.MouseMove +=new MouseEventHandler(glPreviewControl1_MouseMove);
      glPreviewControl1.MouseUp +=new MouseEventHandler(glPreviewControl1_MouseUp);

      bmpControl1 = new Gweronimo.BmpControl();
      bmpControl1.Name = "bmpControl1";
      bmpControl1.Text = "bmpControl1";
      bmpControl1.Location = new System.Drawing.Point(0, 0);
      bmpControl1.Size = mRenderSize;
      //bmpControl1.Anchor = AnchorStyles.Left | AnchorStyles.Top;
      //bmpControl1.Dock = DockStyle.Fill;
      bmpControl1.TabIndex = 0;

      bmpControl1.KeyPress += new KeyPressEventHandler(image_KeyPress);
      bmpControl1.PostProcessingDone +=new EventHandler(bmpControl1_PostProcessingDone);

      this.SuspendLayout();
      previewPanel.Controls.Add(glPreviewControl1);
      renderPanel.Controls.Add(bmpControl1);
      this.ResumeLayout(false);

      propertyGrid1.SelectedObject = RateRacerEngine.mRenderProps;
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
      this.previewPanel = new System.Windows.Forms.Panel();
      this.mainMenu1 = new System.Windows.Forms.MainMenu();
      this.menuItem1 = new System.Windows.Forms.MenuItem();
      this.menuItem2 = new System.Windows.Forms.MenuItem();
      this.sidePanel = new System.Windows.Forms.Panel();
      this.splitter2 = new System.Windows.Forms.Splitter();
      this.propsPanel = new System.Windows.Forms.Panel();
      this.chkAutoRender = new System.Windows.Forms.CheckBox();
      this.btnRender = new System.Windows.Forms.Button();
      this.chkInterpolate = new System.Windows.Forms.CheckBox();
      this.label4 = new System.Windows.Forms.Label();
      this.cbZoom = new System.Windows.Forms.ComboBox();
      this.label3 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.cbHeight = new System.Windows.Forms.ComboBox();
      this.label1 = new System.Windows.Forms.Label();
      this.cbWidth = new System.Windows.Forms.ComboBox();
      this.chkKeepAspect = new System.Windows.Forms.CheckBox();
      this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
      this.cbProps = new System.Windows.Forms.ComboBox();
      this.lblStatus = new System.Windows.Forms.Label();
      this.statusPanel = new System.Windows.Forms.Panel();
      this.renderPanel = new System.Windows.Forms.Panel();
      this.splitter1 = new System.Windows.Forms.Splitter();
      this.sidePanel.SuspendLayout();
      this.propsPanel.SuspendLayout();
      this.statusPanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // previewPanel
      // 
      this.previewPanel.BackColor = System.Drawing.Color.Gray;
      this.previewPanel.Dock = System.Windows.Forms.DockStyle.Top;
      this.previewPanel.DockPadding.All = 8;
      this.previewPanel.Location = new System.Drawing.Point(0, 0);
      this.previewPanel.Name = "previewPanel";
      this.previewPanel.Size = new System.Drawing.Size(240, 160);
      this.previewPanel.TabIndex = 0;
      this.previewPanel.Resize += new System.EventHandler(this.previewPanel_Resize);
      // 
      // mainMenu1
      // 
      this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                              this.menuItem1});
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
      // sidePanel
      // 
      this.sidePanel.BackColor = System.Drawing.Color.Silver;
      this.sidePanel.Controls.Add(this.splitter2);
      this.sidePanel.Controls.Add(this.propsPanel);
      this.sidePanel.Controls.Add(this.previewPanel);
      this.sidePanel.Dock = System.Windows.Forms.DockStyle.Left;
      this.sidePanel.Location = new System.Drawing.Point(0, 0);
      this.sidePanel.Name = "sidePanel";
      this.sidePanel.Size = new System.Drawing.Size(240, 405);
      this.sidePanel.TabIndex = 1;
      // 
      // splitter2
      // 
      this.splitter2.Dock = System.Windows.Forms.DockStyle.Top;
      this.splitter2.Location = new System.Drawing.Point(0, 160);
      this.splitter2.Name = "splitter2";
      this.splitter2.Size = new System.Drawing.Size(240, 3);
      this.splitter2.TabIndex = 3;
      this.splitter2.TabStop = false;
      // 
      // propsPanel
      // 
      this.propsPanel.Controls.Add(this.chkAutoRender);
      this.propsPanel.Controls.Add(this.btnRender);
      this.propsPanel.Controls.Add(this.chkInterpolate);
      this.propsPanel.Controls.Add(this.label4);
      this.propsPanel.Controls.Add(this.cbZoom);
      this.propsPanel.Controls.Add(this.label3);
      this.propsPanel.Controls.Add(this.label2);
      this.propsPanel.Controls.Add(this.cbHeight);
      this.propsPanel.Controls.Add(this.label1);
      this.propsPanel.Controls.Add(this.cbWidth);
      this.propsPanel.Controls.Add(this.chkKeepAspect);
      this.propsPanel.Controls.Add(this.propertyGrid1);
      this.propsPanel.Controls.Add(this.cbProps);
      this.propsPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propsPanel.Location = new System.Drawing.Point(0, 160);
      this.propsPanel.Name = "propsPanel";
      this.propsPanel.Size = new System.Drawing.Size(240, 245);
      this.propsPanel.TabIndex = 2;
      // 
      // chkAutoRender
      // 
      this.chkAutoRender.Location = new System.Drawing.Point(80, 8);
      this.chkAutoRender.Name = "chkAutoRender";
      this.chkAutoRender.TabIndex = 12;
      this.chkAutoRender.Text = "Auto-Render!";
      this.chkAutoRender.CheckedChanged += new System.EventHandler(this.chkAutoRender_CheckedChanged);
      // 
      // btnRender
      // 
      this.btnRender.FlatStyle = System.Windows.Forms.FlatStyle.System;
      this.btnRender.Location = new System.Drawing.Point(8, 8);
      this.btnRender.Name = "btnRender";
      this.btnRender.Size = new System.Drawing.Size(64, 23);
      this.btnRender.TabIndex = 11;
      this.btnRender.Text = "Render!";
      this.btnRender.Click += new System.EventHandler(this.btnRender_Click);
      // 
      // chkInterpolate
      // 
      this.chkInterpolate.Location = new System.Drawing.Point(152, 88);
      this.chkInterpolate.Name = "chkInterpolate";
      this.chkInterpolate.Size = new System.Drawing.Size(88, 24);
      this.chkInterpolate.TabIndex = 10;
      this.chkInterpolate.Text = "Interpolate";
      this.chkInterpolate.CheckedChanged += new System.EventHandler(this.chkInterpolate_CheckedChanged);
      // 
      // label4
      // 
      this.label4.Location = new System.Drawing.Point(8, 120);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(96, 16);
      this.label4.TabIndex = 9;
      this.label4.Text = "Properties:";
      this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // cbZoom
      // 
      this.cbZoom.Location = new System.Drawing.Point(80, 88);
      this.cbZoom.MaxDropDownItems = 20;
      this.cbZoom.MaxLength = 3;
      this.cbZoom.Name = "cbZoom";
      this.cbZoom.Size = new System.Drawing.Size(64, 21);
      this.cbZoom.TabIndex = 8;
      this.cbZoom.Text = "cbZoom";
      this.cbZoom.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.cbZoom_KeyPress);
      this.cbZoom.Validating += new System.ComponentModel.CancelEventHandler(this.cbZoom_Validating);
      this.cbZoom.SelectedIndexChanged += new System.EventHandler(this.cbZoom_SelectedIndexChanged);
      // 
      // label3
      // 
      this.label3.Location = new System.Drawing.Point(8, 88);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(72, 24);
      this.label3.TabIndex = 7;
      this.label3.Text = "Zoom (%):";
      this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // label2
      // 
      this.label2.Location = new System.Drawing.Point(8, 32);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(72, 24);
      this.label2.TabIndex = 6;
      this.label2.Text = "Resolution:";
      this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // cbHeight
      // 
      this.cbHeight.Location = new System.Drawing.Point(168, 56);
      this.cbHeight.MaxDropDownItems = 20;
      this.cbHeight.MaxLength = 5;
      this.cbHeight.Name = "cbHeight";
      this.cbHeight.Size = new System.Drawing.Size(64, 21);
      this.cbHeight.TabIndex = 5;
      this.cbHeight.Text = "cbHeight";
      this.cbHeight.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.cbHeight_KeyPress);
      this.cbHeight.Validating += new System.ComponentModel.CancelEventHandler(this.cbHeight_Validating);
      this.cbHeight.SelectedIndexChanged += new System.EventHandler(this.cbHeight_SelectedIndexChanged);
      // 
      // label1
      // 
      this.label1.Location = new System.Drawing.Point(144, 56);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(24, 21);
      this.label1.TabIndex = 4;
      this.label1.Text = "x";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // cbWidth
      // 
      this.cbWidth.Location = new System.Drawing.Point(80, 56);
      this.cbWidth.MaxDropDownItems = 20;
      this.cbWidth.MaxLength = 5;
      this.cbWidth.Name = "cbWidth";
      this.cbWidth.Size = new System.Drawing.Size(64, 21);
      this.cbWidth.TabIndex = 3;
      this.cbWidth.Text = "cbWidth";
      this.cbWidth.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.cbWidth_KeyPress);
      this.cbWidth.Validating += new System.ComponentModel.CancelEventHandler(this.cbWidth_Validating);
      this.cbWidth.SelectedIndexChanged += new System.EventHandler(this.cbWidth_SelectedIndexChanged);
      // 
      // chkKeepAspect
      // 
      this.chkKeepAspect.Location = new System.Drawing.Point(80, 32);
      this.chkKeepAspect.Name = "chkKeepAspect";
      this.chkKeepAspect.Size = new System.Drawing.Size(120, 24);
      this.chkKeepAspect.TabIndex = 2;
      this.chkKeepAspect.Text = "Keep aspect ratio";
      this.chkKeepAspect.CheckedChanged += new System.EventHandler(this.chkKeepAspect_CheckedChanged);
      // 
      // propertyGrid1
      // 
      this.propertyGrid1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.propertyGrid1.CommandsBackColor = System.Drawing.Color.Gainsboro;
      this.propertyGrid1.CommandsVisibleIfAvailable = true;
      this.propertyGrid1.HelpVisible = false;
      this.propertyGrid1.LargeButtons = false;
      this.propertyGrid1.LineColor = System.Drawing.SystemColors.ScrollBar;
      this.propertyGrid1.Location = new System.Drawing.Point(8, 160);
      this.propertyGrid1.Name = "propertyGrid1";
      this.propertyGrid1.Size = new System.Drawing.Size(224, 80);
      this.propertyGrid1.TabIndex = 1;
      this.propertyGrid1.Text = "propertyGrid1";
      this.propertyGrid1.ToolbarVisible = false;
      this.propertyGrid1.ViewBackColor = System.Drawing.SystemColors.Window;
      this.propertyGrid1.ViewForeColor = System.Drawing.SystemColors.WindowText;
      this.propertyGrid1.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyGrid1_PropertyValueChanged);
      // 
      // cbProps
      // 
      this.cbProps.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.cbProps.Location = new System.Drawing.Point(8, 136);
      this.cbProps.Name = "cbProps";
      this.cbProps.Size = new System.Drawing.Size(224, 21);
      this.cbProps.TabIndex = 0;
      this.cbProps.Text = "cbProps";
      this.cbProps.SelectedIndexChanged += new System.EventHandler(this.cbProps_SelectedIndexChanged);
      // 
      // lblStatus
      // 
      this.lblStatus.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.lblStatus.Location = new System.Drawing.Point(8, 4);
      this.lblStatus.Name = "lblStatus";
      this.lblStatus.Size = new System.Drawing.Size(648, 16);
      this.lblStatus.TabIndex = 2;
      this.lblStatus.Text = "lblStatus";
      this.lblStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // statusPanel
      // 
      this.statusPanel.BackColor = System.Drawing.SystemColors.Control;
      this.statusPanel.Controls.Add(this.lblStatus);
      this.statusPanel.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.statusPanel.Location = new System.Drawing.Point(0, 405);
      this.statusPanel.Name = "statusPanel";
      this.statusPanel.Size = new System.Drawing.Size(664, 24);
      this.statusPanel.TabIndex = 3;
      // 
      // renderPanel
      // 
      this.renderPanel.AutoScroll = true;
      this.renderPanel.BackColor = System.Drawing.Color.White;
      this.renderPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this.renderPanel.Location = new System.Drawing.Point(240, 0);
      this.renderPanel.Name = "renderPanel";
      this.renderPanel.Size = new System.Drawing.Size(424, 405);
      this.renderPanel.TabIndex = 4;
      this.renderPanel.Resize += new System.EventHandler(this.renderPanel_Resize);
      // 
      // splitter1
      // 
      this.splitter1.Location = new System.Drawing.Point(240, 0);
      this.splitter1.Name = "splitter1";
      this.splitter1.Size = new System.Drawing.Size(3, 405);
      this.splitter1.TabIndex = 5;
      this.splitter1.TabStop = false;
      // 
      // MainForm
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(664, 429);
      this.Controls.Add(this.splitter1);
      this.Controls.Add(this.renderPanel);
      this.Controls.Add(this.sidePanel);
      this.Controls.Add(this.statusPanel);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Menu = this.mainMenu1;
      this.Name = "MainForm";
      this.Text = "Rate Racer - a renderer";
      this.Load += new System.EventHandler(this.MainForm_Load);
      this.sidePanel.ResumeLayout(false);
      this.propsPanel.ResumeLayout(false);
      this.statusPanel.ResumeLayout(false);
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
      bmpControl1.setResolution( ref mRenderSize );
      bmpControl1.setZoom( mZoom );
      bmpControl1.setInterpolation( chkInterpolate.Checked );
      adjustPreviewSize();
      adjustRenderSize(mRenderSize, mZoom);
    }

    void adjustPreviewSize()
    {
      int x = 8, y = 8;
      int w = (previewPanel.Width - 2*x), h = (previewPanel.Height - 2*y);

      double rw = h * mAspectRatio, rh = w / mAspectRatio;
      if (rw > w) { rw = w; } else { rh = h; }

      double rx = x + 0.5f * (w - rw), ry = y + 0.5f * (h - rh);
      glPreviewControl1.SetBounds((int)rx, (int)ry, (int)rw, (int)rh);
    }

    void adjustRenderSize(Size renderSize, float zoom)
    {
      if (renderSize != mRenderSize)
      {
        mRenderSize = renderSize;
        if (!chkKeepAspect.Checked) 
        {
          mAspectRatio = (double)mRenderSize.Width / (double)mRenderSize.Height;
        }
        bmpControl1.setResolution( ref mRenderSize );
      }

      if (zoom != mZoom)
      {
        mZoom = zoom;
        bmpControl1.setZoom( mZoom );
      }

      Size zoomedSize = new Size(
        (int)(mZoom * (float)mRenderSize.Width),
        (int)(mZoom * (float)mRenderSize.Height));

      bmpControl1.Size = zoomedSize;

      if (renderPanel.Width > bmpControl1.Width)
        bmpControl1.Left = (renderPanel.Width - bmpControl1.Width) / 2;
      else
        bmpControl1.Left = 0;

      if (renderPanel.Height > bmpControl1.Height)
        bmpControl1.Top = (renderPanel.Height - bmpControl1.Height) / 2;
      else
        bmpControl1.Top = 0;

      renderPanel.PerformLayout();
    }

    void startRender()
    {
      isRendering = true;
      btnRender.Enabled = !chkAutoRender.Checked && !isRendering;
      adjustRenderSize(mRenderSize, mZoom);
      RateRacerEngine.render();

      if (!chkAutoRender.Checked)
      {
        mTimer.Start();
      }
    }

    private void mTimer_Tick(object sender, System.EventArgs e)
    {
      bmpControl1.updateBitmap();
      bmpControl1.Refresh();

      if (updateRenderStatus())
      {
        mTimer.Stop();
      }
    }

    private void bmpControl1_PostProcessingDone(object sender, EventArgs e)
    {
      mTimer.Stop();
      bmpControl1.updateBitmap();
      bmpControl1.Refresh();
      updateRenderStatus();
      isRendering = false;
      btnRender.Enabled = !chkAutoRender.Checked && !isRendering;
    }

    bool updateRenderStatus()
    {
      int percentage = bmpControl1.renderingPercentage();
      float timeSecs = bmpControl1.renderingTimeSeconds();

      lblStatus.Text = "Rendering: " + percentage + "%" +
        " Elapsed time: " + timeString(timeSecs);
      lblStatus.Refresh();

      return (percentage == 100);
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

    private void image_KeyPress(object sender, KeyPressEventArgs e)
    {
      //Console.WriteLine("Key pressed: " + e.KeyChar);
      if (Char.ToUpper(e.KeyChar) == 'R')
      {
        startRender();
      }
    }

    private void renderPanel_Resize(object sender, System.EventArgs e)
    {
      adjustRenderSize(mRenderSize, mZoom);
    }

    private void previewPanel_Resize(object sender, System.EventArgs e)
    {
      adjustPreviewSize();
    }

    private void glPreviewControl1_MouseMove(object sender, MouseEventArgs e)
    {
      if (e.Button == MouseButtons.None) return;
      if (chkAutoRender.Checked)
      {
        if (!isRendering) startRender();
      }
    }

    private void glPreviewControl1_MouseUp(object sender, MouseEventArgs e)
    {
      if (chkAutoRender.Checked)
      {
        startRender();
      }
    }

    void updateResolution(Size renderSize)
    {
      cbWidth.Text  = renderSize.Width.ToString();
      cbHeight.Text = renderSize.Height.ToString();
      if (renderSize != mRenderSize)
      {
        //Console.WriteLine("Changing resolution...");
        adjustRenderSize(renderSize, mZoom);
        adjustPreviewSize();
      }
    }

    void updateResolutionWidth()
    {
      Size renderSize = new Size(mRenderSize.Width, mRenderSize.Height);

      try 
      {
        renderSize.Width = Int32.Parse(cbWidth.Text);
        if (renderSize.Width < 2) renderSize.Width = 2;
        if (renderSize.Width > 10000) renderSize.Width = 10000;
      }
      catch (Exception) {}

      if (chkKeepAspect.Checked)
      {
        renderSize.Height = (int)((double)renderSize.Width / mAspectRatio);
      }

      updateResolution(renderSize);
    }

    void updateResolutionHeight()
    {
      Size renderSize = new Size(mRenderSize.Width, mRenderSize.Height);

      try 
      {
        renderSize.Height = Int32.Parse(cbHeight.Text);
        if (renderSize.Height < 2) renderSize.Height = 2;
        if (renderSize.Height > 10000) renderSize.Height = 10000;
      }
      catch (Exception) {}

      if (chkKeepAspect.Checked)
      {
        renderSize.Width = (int)((double)renderSize.Height * mAspectRatio);
      }

      updateResolution(renderSize);
    }

    private void cbWidth_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      updateResolutionWidth();
    }

    private void cbWidth_Validating(object sender, System.ComponentModel.CancelEventArgs e)
    {
      updateResolutionWidth();
      //e.Cancel = true;
    }

    private void cbWidth_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
    {
      if (e.KeyChar == '\r')
      {
        updateResolutionWidth();
      }
    }

    private void cbHeight_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      updateResolutionHeight();
    }

    private void cbHeight_Validating(object sender, System.ComponentModel.CancelEventArgs e)
    {
      updateResolutionHeight();
      //e.Cancel = true;
    }

    private void cbHeight_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
    {
      if (e.KeyChar == '\r')
      {
        updateResolutionHeight();
      }
    }

    private void chkKeepAspect_CheckedChanged(object sender, System.EventArgs e)
    {
      //if (this.Visible)
      {
      }
    }

    void updateZoom()
    {
      int percentage = 100;

      try 
      {
        percentage = Int32.Parse(cbZoom.Text);
        if (percentage <   1) percentage =   1;
        if (percentage > 800) percentage = 800;
      }
      catch (Exception) {}

      cbZoom.Text = percentage.ToString();;
      float zoom = 0.01f * percentage;
      adjustRenderSize(mRenderSize, zoom);
    }

    private void cbZoom_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      updateZoom();
    }

    private void cbZoom_Validating(object sender, System.ComponentModel.CancelEventArgs e)
    {
      updateZoom();
    }

    private void cbZoom_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
    {
      if (e.KeyChar == '\r')
      {
        updateZoom();
      }
    }

    private void chkInterpolate_CheckedChanged(object sender, System.EventArgs e)
    {
      if (this.Visible)
      {
        bmpControl1.setInterpolation( chkInterpolate.Checked );
        bmpControl1.Invalidate();
      }
    }

    private void cbProps_SelectedIndexChanged(object sender, System.EventArgs e)
    {
    
    }

    private void propertyGrid1_PropertyValueChanged(object s, System.Windows.Forms.PropertyValueChangedEventArgs e)
    {
      glPreviewControl1.Invalidate();
    }

    private void chkAutoRender_CheckedChanged(object sender, System.EventArgs e)
    {
      if (this.Visible)
      {
        btnRender.Enabled = !chkAutoRender.Checked && !isRendering;
      }
      if (chkAutoRender.Checked)
      {
        if (!isRendering) startRender();
      }
    }

    private void btnRender_Click(object sender, System.EventArgs e)
    {
      startRender();
    }

  }
}
