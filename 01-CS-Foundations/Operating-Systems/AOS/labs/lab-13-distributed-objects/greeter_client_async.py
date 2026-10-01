import asyncio
import grpc
import time

import greeter_pb2
import greeter_pb2_grpc

async def run():
    async with grpc.aio.insecure_channel('localhost:50051') as channel:
        stub = greeter_pb2_grpc.GreeterStub(channel)
        
        times = []
        response = None
        for _ in range(10):
            start = time.monotonic_ns()
            response = await stub.SayHello(greeter_pb2.HelloRequest(name='async client'))
            end = time.monotonic_ns()
            times.append(end - start)
            
        times.sort()
        median = times[5]
        
        print("Greeter client received: " + response.message)
        print(f"gRPC async invocation median time (10 runs): {median} ns")

if __name__ == '__main__':
    asyncio.run(run())
