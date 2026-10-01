PYTHON = /opt/aos-venv/bin/python
JAVAC = javac
JAVA = java

all: rmi grpc

rmi: Hello.class Server.class Client.class

Hello.class: Hello.java
	$(JAVAC) Hello.java

Server.class: Server.java Hello.class
	$(JAVAC) Server.java

Client.class: Client.java Hello.class
	$(JAVAC) Client.java

grpc: greeter_pb2.py

greeter_pb2.py: greeter.proto
	$(PYTHON) -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. greeter.proto

run: all
	@echo "--- Running Subcontract (Python) ---"
	$(PYTHON) subcontract.py
	@echo ""
	@echo "--- Running Java RMI ---"
	-pkill -9 rmiregistry 2>/dev/null || true
	-pkill -f "java Server" 2>/dev/null || true
	rmiregistry 1099 & echo $$! > rmi.pid; sleep 2
	$(JAVA) Server & echo $$! > server.pid; sleep 2
	$(JAVA) Client
	kill -9 `cat server.pid` `cat rmi.pid` || true
	rm -f server.pid rmi.pid
	@echo ""
	@echo "--- Running gRPC ---"
	-pkill -f greeter_server.py 2>/dev/null || true
	$(PYTHON) greeter_server.py & echo $$! > grpc.pid; sleep 2
	$(PYTHON) greeter_client_sync.py
	$(PYTHON) greeter_client_async.py
	kill -9 `cat grpc.pid` || true
	rm -f grpc.pid

test: all
	@echo "Testing Subcontract..."
	@$(PYTHON) subcontract.py > subcontract_test.out
	@grep -q "Singleton output: Worker A processed: test data" subcontract_test.out && echo "PASS: Subcontract Singleton output correct" || (echo "FAIL: Subcontract Singleton" && exit 1)
	@grep -q "Replicated output: Worker A processed: test data" subcontract_test.out && echo "PASS: Subcontract Replicated output correct" || (echo "FAIL: Subcontract Replicated" && exit 1)
	@echo "Testing Java RMI..."
	@-pkill -9 rmiregistry 2>/dev/null || true
	@-pkill -f "java Server" 2>/dev/null || true
	@rmiregistry 1099 & echo $$! > rmi.pid; sleep 2
	@$(JAVA) Server & echo $$! > server.pid; sleep 2
	@$(JAVA) Client > rmi_test.out
	@kill -9 `cat server.pid` `cat rmi.pid` 2>/dev/null || true
	@rm -f server.pid rmi.pid
	@grep -q "response: Hello, world from Java RMI!" rmi_test.out && echo "PASS: Java RMI output correct" || (echo "FAIL: Java RMI" && exit 1)
	@echo "Testing gRPC..."
	@-pkill -f greeter_server.py 2>/dev/null || true
	@$(PYTHON) greeter_server.py & echo $$! > grpc.pid; sleep 2
	@$(PYTHON) greeter_client_sync.py > grpc_sync_test.out
	@$(PYTHON) greeter_client_async.py > grpc_async_test.out
	@kill -9 `cat grpc.pid` 2>/dev/null || true
	@rm -f grpc.pid
	@grep -q "Greeter client received: Hello, sync client!" grpc_sync_test.out && echo "PASS: gRPC Sync output correct" || (echo "FAIL: gRPC Sync" && exit 1)
	@grep -q "Greeter client received: Hello, async client!" grpc_async_test.out && echo "PASS: gRPC Async output correct" || (echo "FAIL: gRPC Async" && exit 1)
	@echo "PASS: All tests passed."

clean:
	rm -f *.class *.pyc *_pb2*.py *.out *.pid
	rm -rf __pycache__
