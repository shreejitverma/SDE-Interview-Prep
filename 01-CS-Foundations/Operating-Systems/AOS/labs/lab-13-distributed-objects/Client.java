import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

public class Client {
    private Client() {}

    public static void main(String[] args) {
        String host = (args.length < 1) ? "localhost" : args[0];
        try {
            Registry registry = LocateRegistry.getRegistry(host, 1099);
            Hello stub = (Hello) registry.lookup("Hello");
            
            long[] times = new long[10];
            String response = "";
            for (int i = 0; i < 10; i++) {
                long start = System.nanoTime();
                response = stub.sayHello();
                long end = System.nanoTime();
                times[i] = end - start;
            }
            java.util.Arrays.sort(times);
            long median = times[5]; // simple median for 10 elements
            
            System.out.println("response: " + response);
            System.out.println("RMI invocation median time (10 runs): " + median + " ns");
        } catch (Exception e) {
            System.err.println("Client exception: " + e.toString());
            e.printStackTrace();
            System.exit(1);
        }
    }
}
