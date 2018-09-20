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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Runtime.InteropServices;
using System.Threading;
using System.Collections.ObjectModel;
using Microsoft.Win32;

namespace WPFCallDll
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        //[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void UdpCbDelegate(int cmd, string srcAddr, string msg);
        public delegate void FileCbDelegate(int state, int type, int process, string filename);

        [DllImport("WinsCore.dll", EntryPoint = "initSocket", CallingConvention = CallingConvention.Cdecl)]
        extern static void initSocket();
        [DllImport("WinsCore.dll", EntryPoint = "startReceive", CallingConvention = CallingConvention.Cdecl)]
        extern static void startReceive(string path);
        [DllImport("WinsCore.dll", EntryPoint = "sendFile", CallingConvention = CallingConvention.Cdecl)]
        extern static void sendFile(string address, string path);
        [DllImport("WinsCore.dll", EntryPoint = "sendUdpData", CallingConvention = CallingConvention.Cdecl)]
        extern static void sendUdpData(int cmd, String destAddr, String msg);
        [DllImport("WinsCore.dll", EntryPoint = "setUdpNotify", CallingConvention = CallingConvention.Cdecl)]
        extern static void setUdpNotify(UdpCbDelegate udpDelegate);
        [DllImport("WinsCore.dll", EntryPoint = "setFileNotify", CallingConvention = CallingConvention.StdCall)]
        extern static void setFileNotify(FileCbDelegate fileDelegate);

        private UdpCbDelegate udpDelegate = new UdpCbDelegate(udpNotify);
        private FileCbDelegate fileDelegate = new FileCbDelegate(fileNotify);

        private static MainWindow window;
        private static Thread fileThread;
        private static ThreadStart threadStart = new ThreadStart(setProgress);
        private static int progressValue;
        private static string destIp;

        private delegate void IpOperateEvent(string ip);

        public MainWindow()
        {
            InitializeComponent();
            setUdpNotify(udpDelegate);
            setFileNotify(fileDelegate);
            //GC.KeepAlive(udpDelegate);
            //GC.KeepAlive(fileDelegate);
            initSocket();
            window = this;
        }

        private void Select_File_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            if (dialog.ShowDialog() == true)
            {
                this.fileName.Text = dialog.FileName; 
            }
        }

        private void Send_Req_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (destIp == null)
                {
                    this.log.Text = "Tip: 请先选择局域网用户";
                    return;
                }
                sendUdpData(6, destIp, " ");
            }
            catch (Exception ex)
            {
                ex.ToString();
            }
        }

        private static void udpNotify(int cmd, string srcAddr, string msg)
        {
            if (cmd == 1 || cmd == 3) //用户上线
            {
                window.log.Dispatcher.Invoke(() => window.log.Text = "Tip: " + srcAddr);
                ThreadPool.QueueUserWorkItem(delegate
                {
                    window.Dispatcher.Invoke(new IpOperateEvent(addIp), srcAddr);
                });
            } else if (cmd == 2)
            {
                ThreadPool.QueueUserWorkItem(delegate
                {
                    window.Dispatcher.Invoke(new IpOperateEvent(deleteIp), srcAddr);
                });
            } else if (cmd == 5) //对方拒绝接收文件
            {
                window.log.Dispatcher.Invoke(() => window.log.Text = "Tip: 对方拒绝了您的文件发送请求！");
            } else if (cmd == 4) //对方允许接收文件
            {
                window.log.Dispatcher.Invoke(() => window.log.Text = "Tip: 对方准备接收您的文件！");
                try
                {
                    sendFile(srcAddr, window.fileName.Dispatcher.Invoke(()=>window.fileName.Text));
                }
                catch (Exception ex)
                {
                    ex.ToString();
                }
            } else if (cmd == 6) //文件发送请求
            {
                MessageBoxResult result = MessageBox.Show("是否接收" + srcAddr + "发来的文件", "文件发送请求", MessageBoxButton.OKCancel, MessageBoxImage.Question);
                if (result == MessageBoxResult.OK)
                {
                    try
                    {
                        sendUdpData(4, srcAddr, " ");
                        startReceive("F:/file");
                    }
                    catch (Exception ex)
                    {
                        ex.ToString();
                    }
                    window.log.Dispatcher.Invoke(() => window.log.Text = "Tip: 准备接收对方文件！");
                } else
                {
                    try
                    {
                        sendUdpData(5, srcAddr, " ");
                    }
                    catch (Exception ex)
                    {
                        ex.ToString();
                    }
                    window.log.Dispatcher.Invoke(() => window.log.Text = "Tip: 您拒绝了对方的文件发送请求！");
                }
            }
        }

        private static void addIp(string ip)
        {
            bool isAdd = true;
            ItemCollection collection = window.lanlist.Items;
            foreach (string temp in collection)
            {
                if (ip.Equals(temp))
                {
                    isAdd = false;
                    break;
                }
            }
            if (isAdd)
            {
                window.lanlist.Dispatcher.Invoke(() => window.lanlist.Items.Add(ip));
            }
          
        }

        private static void deleteIp(string ip)
        {
            window.lanlist.Dispatcher.Invoke(() => window.lanlist.Items.Remove(ip));
        }

        private static void fileNotify(int state, int type, int process, string filename)
        {
            if (type == 7)
            {
                window.fileTip.Dispatcher.Invoke(() => window.fileTip.Text = "正在发送：" + filename);
            }
            else if (type == 8)
            {
                window.fileTip.Dispatcher.Invoke(() => window.fileTip.Text = "正在接收：" + filename);
            }
            if (state == 9) //开始上传
            {
                //启动线程
                if (fileThread == null)
                {
                    fileThread = new Thread(threadStart);
                    fileThread.Start();
                }
            }
            else if (state == 11) //上传成功
            {
                if (fileThread != null)
                {
                    fileThread.Abort();
                    fileThread = null;
                }
                if (type == 7)
                {
                    window.fileTip.Dispatcher.Invoke(() => window.fileTip.Text = "发送成功：" + filename);
                }
                else if (type == 8)
                {
                    window.fileTip.Dispatcher.Invoke(() => window.fileTip.Text = "接收成功：" + filename);
                }
            }
            else if (state == 12) //上传中
            {
                progressValue = process;
                threadStart();
            }
        }

        private static void setProgress()
        {
            window.progress.Dispatcher.Invoke(() => window.progress.Value = progressValue);
        }

        private void ProgressBar_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {

        }

        private void lanlist_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            destIp = (string)this.lanlist.SelectedValue;
        }

    }

}
