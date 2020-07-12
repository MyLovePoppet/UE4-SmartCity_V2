# 发送数据包的代码
## 接收方的端口地址
```java
enum EOperationPort {
    FLY_MODE_DRAG(9098),
    FLY_MODE_SCALE(9097),
    FLY_MODE_START_PORT(9096),//Start信号与Drag信号放一起处理
    FLY_MODE_ROTATE(9096);

    final int port;

    EOperationPort(int port) {
        this.port = port;
    }
}
```
## 真正的Udp发送的代码
```Java
class SendData implements Runnable {
    public final static InetAddress remoteAddress;
    //public final static int remotePort;
    public final static DatagramSocket datagramSocket;

    static {
        InetAddress remoteAddress1;
        DatagramSocket datagramSocket1;
        try {
            remoteAddress1 = InetAddress.getByName("192.168.50.52");
            datagramSocket1 = new DatagramSocket();
        } catch (UnknownHostException | SocketException e) {
            e.printStackTrace();
            //Log.w("HTTPUtils", e);
            remoteAddress1 = null;
            datagramSocket1 = null;
        }
        datagramSocket = datagramSocket1;
        remoteAddress = remoteAddress1;
        //remotePort = 9098;
    }

    String str;
    EOperationPort operationPort;

    public SendData(String str, EOperationPort operationPort) {
        this.str = str;
        this.operationPort = operationPort;
    }

    @Override
    public void run() {
        byte[] bytes = str.getBytes();
        //如果是开始则对每个端口都进行发一个数据包
        if (operationPort == EOperationPort.FLY_MODE_START_PORT) {
            for (EOperationPort eOperationPort : EOperationPort.values()) {
                if (eOperationPort != operationPort) {
                    DatagramPacket datagramPacket = new DatagramPacket(bytes, bytes.length, remoteAddress, eOperationPort.port);
                    if (datagramSocket != null) {
                        try {
                            datagramSocket.send(datagramPacket);
                            //Log.i("Send Thread", "Send OK");
                        } catch (IOException e) {
                            e.printStackTrace();
                            //Log.w("Send Thread",e);
                        }
                    }
                }
            }
        }
        //否则直接发就可以了
        else {
            DatagramPacket datagramPacket = new DatagramPacket(bytes, bytes.length, remoteAddress, operationPort.port);
            if (datagramSocket != null) {
                try {
                    datagramSocket.send(datagramPacket);
                    //Log.i("Send Thread", "Send OK");
                } catch (IOException e) {
                    e.printStackTrace();
                    //Log.w("Send Thread",e);
                }
            }
        }
    }
}
```
# NOI接收数据包
## 接收数据包的类型
```java
import java.util.HashMap;
import java.util.Map;

public enum ESendBackMessageType {
    //由飞行模式转化为地面模式
    MESSAGE_MODE_CHANGE_TO_GROUND,
    //由地面模式转化为飞行模式，暂时用不上
    MESSAGE_MODE_CHANGE_TO_FLY;
    //使用一个哈希表来存
    static Map<String, ESendBackMessageType> sendBackMessageMap;
    //初始化
    static {
        sendBackMessageMap = new HashMap<>(2);
        sendBackMessageMap.put("{type=\"MODE_CHANGE_TO_GROUND\"}", MESSAGE_MODE_CHANGE_TO_GROUND);
        sendBackMessageMap.put("{type=\"MODE_CHANGE_TO_FLY\"}", MESSAGE_MODE_CHANGE_TO_FLY);
    }
    //由Json字符串获取对应的Type
    static ESendBackMessageType SendBackStrToSenBackEnum(String sendBackStr) {
        return sendBackMessageMap.get(sendBackStr);
    }
}
```
## NOI作为服务器
```java
import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.Iterator;
import java.util.concurrent.TimeUnit;

public class ReceiveMsgThread_WithNOI implements Runnable {
    private final DatagramChannel datagramChannel;

    public ReceiveMsgThread_WithNOI(DatagramChannel datagramChannel) {
        this.datagramChannel = datagramChannel;
    }

    @Override
    public void run() {
        try {
            Selector selector = Selector.open();
            datagramChannel.register(selector, SelectionKey.OP_READ);
            ByteBuffer byteBuffer = ByteBuffer.allocate(1024);
            //接收数据
            while (selector.select() > 0) {
                Iterator<SelectionKey> selectionKeyIterator = selector.selectedKeys().iterator();
                while (selectionKeyIterator.hasNext()) {
                    SelectionKey selectionKey = selectionKeyIterator.next();
                    if (selectionKey.isReadable()) {
                        DatagramChannel datagramChannel1 = (DatagramChannel) selectionKey.channel();
                        System.out.println("接收来自" + datagramChannel1.getRemoteAddress() + "的消息:");
                        datagramChannel1.receive(byteBuffer);
                        byteBuffer.flip();

                        //获取JsonStr
                        String JsonStr = new String(byteBuffer.array(), 0, byteBuffer.limit());
                        //处理JsonStr，更改样式等操作
                        ESendBackMessageType sendBackType = ESendBackMessageType.SendBackStrToSenBackEnum(JsonStr);
                        //如果类型不对则忽略
                        if (sendBackType == null)
                            continue;
                        //根据不同的操作来进行对应的操作
                        switch (sendBackType) {
                            //将模式由飞行模式转化为地面模式
                            case MESSAGE_MODE_CHANGE_TO_GROUND:

                                break;
                            //模式由地面模式转化为飞行模式
                            case MESSAGE_MODE_CHANGE_TO_FLY:

                                break;

                            default:
                                break;
                        }
                        byteBuffer.clear();
                    }
                    selectionKeyIterator.remove();
                }
            }
            selector.close();
            datagramChannel.close();
        } catch (ClosedByInterruptException e) {
            //System.out.println("ClosedByInterruptException");
            //Log.i("Running state","ClosedByInterruptException");
        } catch (AsynchronousCloseException e) {
            //System.out.println("AsynchronousCloseException");
            //Log.i("Running state","AsynchronousCloseException");
        } catch (IOException e) {
            throw new RuntimeException(e);
            //Log.w("Runtime exception",e);
        }
        //Log.i("Running State","Exiting NIOUdpThread");
        System.out.println("Exiting NIOBlocked.run() " + this);
    }

    public static void main(String[] args) throws Exception {
        //被阻塞的nio通道会自动地响应中断
        //ExecutorService exec = Executors.newCachedThreadPool();
        //ServerSocket server = new ServerSocket(8080);

        //而普通的阻塞的IO则很难做到实现阻塞状态下的中断，故使用NOI来进行操作

        //程序开始时调用
        InetSocketAddress inetSocketAddress = new InetSocketAddress(9077);
        DatagramChannel datagramChannel = DatagramChannel.open();
        datagramChannel.bind(inetSocketAddress);
        datagramChannel.configureBlocking(false);
        Thread updThread = new Thread(new ReceiveMsgThread_WithNOI(datagramChannel));
        updThread.start();


        //程序执行
        TimeUnit.SECONDS.sleep(6000);

        //程序结束时调用
        updThread.interrupt();
        datagramChannel.close();
    }
}
```
## BIO的Udp接收
```java
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

public class ReceiveMsgThread_WithBOI implements Runnable {
    final DatagramSocket datagramSocket;

    public ReceiveMsgThread_WithBOI(DatagramSocket datagramSocket) {
        this.datagramSocket = datagramSocket;
    }

    @Override
    public void run() {
        byte[] bytes = new byte[1024];
        DatagramPacket datagramPacket = new DatagramPacket(bytes, 0, bytes.length);
        try {
            while (true) {
                datagramSocket.receive(datagramPacket);
                //获取接收到的字符串
                String JsonStr = new String(datagramPacket.getData());
                //获取对应的类型
                ESendBackMessageType sendBackType = ESendBackMessageType.SendBackStrToSenBackEnum(JsonStr);
                //如果类型不对则忽略
                if (sendBackType == null)
                    continue;
                //根据不同的操作来进行对应的操作
                switch (sendBackType) {
                    //将模式由飞行模式转化为地面模式
                    case MESSAGE_MODE_CHANGE_TO_GROUND:

                        break;
                    //模式由地面模式转化为飞行模式
                    case MESSAGE_MODE_CHANGE_TO_FLY:

                        break;

                    default:
                        break;
                }

            }
        } catch (IOException e) {
            //如果是主线程关闭引起的则不会有问题
            if (e.getClass() == SocketException.class)
                //Log.i(...)
                ;
                //e.printStackTrace();
            else//否则是真的出现异常了
                e.printStackTrace();
        }
    }

    public static void main(String[] args) throws SocketException, InterruptedException {
        final int MESSAGE_SEND_BACK_PORT = 9077;
        //程序开始时调用
        DatagramSocket datagramSocket = new DatagramSocket(MESSAGE_SEND_BACK_PORT);
        Thread udpThread = new Thread(new ReceiveMsgThread_WithBOI(datagramSocket));
        udpThread.start();

        //程序运行时
        Thread.sleep(5000);

        //程序结束时关闭Socket会自动中断上述线程，但是会引发一个异常
        datagramSocket.close();
    }
}
```