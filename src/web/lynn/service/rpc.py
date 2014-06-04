# -*- coding: utf-8 -*-
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.server import TServer
from thrift.transport.TTransport import TTransportException

class Rpc:
    def __init__(self, T = '', P = ''):
        self.T = T
        self.P = P
        
    def Listen(self, port):
        handler = self.T()
        processor = self.P(handler)
        #注意传入参数的时候一定要用port=port，否则port就被为ip了，就会报属性错误
        transport = TSocket.TServerSocket(port=port)
        tfactory = TTransport.TBufferedTransportFactory()
        pfactory = TBinaryProtocol.TBinaryProtocolFactory()
        #这个是单线程的服务器
        #server = TServer.TSimpleServer(processor, transport, tfactory, pfactory)
        server = TServer.TThreadedServer(processor, transport, tfactory, pfactory)
        server.serve()

    def GetProxy(self, endpoint):
        [ip, port] = endpoint.split(":")
        transport = TSocket.TSocket(ip, int(port))
        transport = TTransport.TBufferedTransport(transport)
        protocol = TBinaryProtocol.TBinaryProtocol(transport)
        proxy = self.T(protocol)
        transport.open()
        return proxy
        
if __name__ == '__main__':
    import sys
    #python不支持gen-py 中的特殊字符,所以将文件夹的名字改为gen
    #主要是我想取文件名字为vm_worker.py，这个与自动生成的文件名冲突了 
    sys.path.append('./gen')
    
    print '1111111111'
    from gen.worker import Worker
    from gen.worker.ttypes import *
    from gen.vm_worker import VMWorker
    from gen.vm_worker.ttypes import *
    from gen.master import Master
    from gen.master.ttypes import *
    
    worker_endpoint = '192.168.1.61:9998'
    worker_client = Rpc(T = Worker.Client).GetProxy(worker_endpoint)
    print '2222'
    vmid = '01-01'
    try :
        if not worker_client.ReportVMState(vmid, VMState.VM_RUNNING_AVAILABLE):
            print "error1"
    except TTransportException, e:
        print e
    
    '''
    from gen.vm_worker import VMWorker
    from gen.vm_worker.ttypes import *
    from vm_worker import VMWorkerHandler
    port = 10000
    Rpc(T = VMWorkerHandler, P = VMWorker.Processor).Listen(int(port))
    '''
