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
    private Gweronimo.BmpControl bmpControl1;

    /// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public RenderForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
      this.bmpControl1 = new Gweronimo.BmpControl();
      // 
      // bmpControl1
      // 
      this.bmpControl1.Name = "bmpControl1";
      this.bmpControl1.Text = "bmpControl1";
      this.bmpControl1.Location = new System.Drawing.Point(24, 32);
      this.bmpControl1.Size = new System.Drawing.Size(200, 200);
      this.bmpControl1.Dock = DockStyle.Fill;
      this.bmpControl1.TabIndex = 0;

      this.Controls.Add(this.bmpControl1);

      bmpControl1.createBitmap();
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
      // 
      // RenderForm
      // 
      this.AutoScale = false;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(292, 270);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "RenderForm";
      this.Text = "RenderForm";
      this.Load += new System.EventHandler(this.RenderForm_Load);

    }
		#endregion

    public void startRendering()
    {
      if (this.InvokeRequired) {
        Console.WriteLine("invoke: " + System.Threading.Thread.CurrentThread.GetHashCode());
        MethodInvoker mi = new MethodInvoker(startRendering);
        this.BeginInvoke(mi);
      }
      else {
        Console.WriteLine("render: " + System.Threading.Thread.CurrentThread.GetHashCode());
        if (!Visible) Show();
        RateRacerEngine.render();
      }
    }

    private void RenderForm_Load(object sender, System.EventArgs e)
    {
      startRendering();
    }
	}
}
