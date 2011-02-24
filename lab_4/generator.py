#!/usr/bin/env python
# 
# Workload generator for a web server
#       Daniel Zappala
#       2008

import gc
import httplib
import optparse
import random
import socket
import sys
import threading
import thread
import time
import traceback

class Logger:
    def __init__(self):
        self.sem = threading.Semaphore()

    def log(self,line):
        self.sem.acquire()
        print line
        sys.stdout.flush()
        self.sem.release()

class Connection:
    def __init__(self,host,port):
        self.host = host
        self.port = port
        self.conn = httplib.HTTPConnection(self.host, self.port)

    def get(self,uri):
        self.conn.request("GET",uri)
        resp = self.conn.getresponse()
        return resp

    def close(self):
        self.conn.close()

class Session(threading.Thread):
    def __init__(self, number, host, port, chooser, logger):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.number = number
        self.host = host
        self.port = port
        self.chooser = chooser
        self.logger = logger
        self.conn = Connection(host,port)

    def numPages(self):
        return 1

    def numObjects(self):
        alpha = 2
        return int(round(random.paretovariate(alpha)))

    def pause(self):
        alpha = 1.5
        return random.paretovariate(alpha)


    def request(self,uri):
        start = time.time()
        buflen = 0
        try:
            resp = self.conn.get(uri)
            if int(resp.status) != 200:
                self.logger.log("%d %s %s %s -" % (self.number,uri,resp.status,resp.reason.strip()))
                return 0
            else:
                length = resp.getheader('content-length')
                mimetype = resp.getheader('content-type')
                if not length:
                    self.logger.log("%d %s 600 NoContentLength 0 0" % (self.number,uri))
                    return 0
                elif not length.isdigit():
                    self.logger.log("%d %s 601 BadContentLength 0 0" % (self.number,uri))
                    return 0
                else:
                    length = int(length)

                    buflen = 0
                    while True:
                        buf = resp.read(65000)
                        if len(buf) > 0:
                            buflen += len(buf)
                        else:
                            self.logger.log("%d %s 602 MessageShort 0 0" % (self.number,uri))
                            return 0
                        if buflen > length:
                            self.logger.log("%d %s 603 MessageLong 0 0" % (self.number,uri))
                            return 0
                        if buflen == length:
                            resp.close()
                            break
                        
        except:
            error = sys.exc_info()[0]
            reason = sys.exc_info()[1]
            # traceback.print_exc()
            self.logger.log("%d %s 610 %s 0 0" % (self.number, uri,str(error)+str(reason)))
            return 0
        elapsedtime = time.time() - start
        self.logger.log("%d %s 200 OK %d %f" % (self.number,uri,buflen,elapsedtime))
        return buflen

    def run(self):
        start = time.time()
        size = 0
        pages = self.numPages()
        for i in xrange(0,pages):
            # get first object
            uri = "/file001.txt"
            size += self.request(uri)
            # get rest of objects
            objects = self.numObjects()
            threads = []
            for i in xrange(0,objects-1):
                uri = "/" + self.chooser.chooseFile()
                size += self.request(uri)
            # sleep
            # pause = self.pause()
            # time.sleep(pause)
        self.conn.close()
        now = time.time()
        elapsedtime = now - start
        # self.logger.log("%d total OK %d %f" % (self.number,size,elapsedtime))

class Chooser:
    def __init__(self):
        self.total = 1000
        self.pop = self.generatePops()

    def generatePops(self):
        pop = []
        # zipf alpha constant
        alpha = 1.0
        # compute normalization constant
        c = 0.0
        for i in xrange(1,self.total+1):
            c = c + (1.0 / (i**alpha))
        c = 1.0 / c
        # choose popularity of each file
        for i in xrange(1,self.total+1):
            pop.append(c/(i**alpha))
        return pop

    def chooseFile(self):
        x = random.random()
        sum = 0
        for p in self.pop:
            sum = sum + p
            if sum >= x:
                break
        return 'file'+str(self.pop.index(p)).zfill(3)+'.txt'


class WorkloadGenerator:
    def __init__(self, host, port, load):
        self.host = host
        self.port = port
        self.load = load
        self.chooser = Chooser()
        self.logger = Logger()

    def pause(self):
        return random.expovariate(self.load)

    def startTotal(self,total):
        sessionNumber = 0
        threads = []
        for i in xrange(0,total):
            try:
                s = Session(sessionNumber,self.host,self.port,self.chooser,self.logger)
                s.start()
            except:
                print "Too many threads"
                return
            threads.append(s)
            # sleep
            pause = self.pause()
            time.sleep(pause)
            sessionNumber += 1
            
        for t in threads:
            t.join()

    def startDuration(self,duration):
        start = time.time()
        sessionNumber = 0
        threads = []
        while True:
            now = time.time()
            if (now - start) > duration:
                break
            try:
                s = Session(sessionNumber,self.host,self.port,self.chooser,self.logger)
                s.start()
            except:
                continue
            threads.append(s)
            # sleep
            pause = self.pause()
            time.sleep(pause)
            sessionNumber += 1
            
        for t in threads:
            t.join()


if __name__ == "__main__":
    gc.set_debug(gc.DEBUG_LEAK)
    parser = optparse.OptionParser(usage="%prog -h [host] -p [port] -l [load (sessions/s)] -d [duration (seconds)] -t [total (sessions)] -n [seed]", version="%prog 1.0")
    parser.add_option("-s", "--server", dest="host", type="str", default="localhost",
                      help= "server to connect to")
    parser.add_option("-p", "--port", dest="port", type="int", default=80,
                      help= "port number to connect to")
    parser.add_option("-l", "--load", dest="load", type="int", default=1,
                      help= "number of sessions to create per second")
    parser.add_option("-t", "--total", dest="total", type="int", default=None,
                      help= "total number of sessions to generate")
    parser.add_option("-d", "--duration", dest="duration", type="int", default=1,
                      help= "total number of seconds to generate load")
    parser.add_option("-n", "--seed", dest="seed", type="int", default=100,
                      help= "random number generator seed")
    
    (options, args) = parser.parse_args()

    # seed random number generator
    random.seed(options.seed)

    thread.stack_size(50000)

    # launch generator
    wg = WorkloadGenerator(options.host, options.port, options.load)
    if options.total:
        wg.startTotal(options.total)
    else:
        wg.startDuration(options.duration)

