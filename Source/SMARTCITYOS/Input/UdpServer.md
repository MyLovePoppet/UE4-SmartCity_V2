```java
//发送数据包的代码
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

```java
import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.*;
import java.util.Iterator;
import java.util.concurrent.TimeUnit;

public class NIOUdpThread implements Runnable {
    private final DatagramChannel datagramChannel;

    public NIOUdpThread(DatagramChannel datagramChannel) {
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
                        //System.out.println("接收来自" + datagramChannel1.getRemoteAddress() + "的消息:");
                        datagramChannel1.receive(byteBuffer);
                        byteBuffer.flip();

                        //获取JsonStr
                        String JsonStr=new String(byteBuffer.array(), 0, byteBuffer.limit());
                        //处理JsonStr，更改样式等操作
                        //...

                        byteBuffer.clear();
                    }
                    selectionKeyIterator.remove();
                }
            }
            selector.close();
            datagramChannel.close();
        } catch (ClosedByInterruptException e) {
            System.out.println("ClosedByInterruptException");
        } catch (AsynchronousCloseException e) {
            System.out.println("AsynchronousCloseException");
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        System.out.println("Exiting NIOBlocked.run() " + this);
    }
    public static void main(String[] args) throws Exception {
        //被阻塞的nio通道会自动地响应中断
        //而普通的阻塞的IO则很难做到实现阻塞状态下的中断，故使用NOI来进行操作

        //程序开始时调用
        InetSocketAddress inetSocketAddress = new InetSocketAddress(9099);
        DatagramChannel datagramChannel = DatagramChannel.open();
        datagramChannel.bind(inetSocketAddress);
        datagramChannel.configureBlocking(false);
        Thread updThread=new Thread(new NIOUdpThread(datagramChannel));
        updThread.start();

        //程序执行
        TimeUnit.SECONDS.sleep(8);

        //程序结束时调用
        datagramChannel.close();
    }
}
```