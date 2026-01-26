/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Asynchronous Programming in Java
 * Description: Using CompletableFuture to chain async tasks without blocking the main thread.
 *           Critical for high-throughput Microservices (Spring Boot).
 */

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.TimeUnit;

public class AsyncDemo {

    public static void main(String[] args) {
        System.out.println("Main: Starting...");

        // 1. Run Async Task
        CompletableFuture<String> future = CompletableFuture.supplyAsync(() -> {
            sleep(1000);
            System.out.println("Task 1: Fetching User Data...");
            return "User: Alice";
        });

        // 2. Chain Task (Process Data)
        CompletableFuture<String> resultFuture = future.thenApply(user -> {
            System.out.println("Task 2: Enriching " + user);
            return user + " (Premium)";
        });

        // 3. Exception Handling and Final Consumer
        resultFuture.exceptionally(ex -> {
            System.out.println("Error: " + ex.getMessage());
            return "Unknown User";
        }).thenAccept(finalResult -> {
            System.out.println("Final Result: " + finalResult);
        });

        System.out.println("Main: Non-blocking execution continues...");
        
        // Wait for async tasks to finish (just for demo purposes)
        sleep(2000);
    }

    private static void sleep(int ms) {
        try {
            TimeUnit.MILLISECONDS.sleep(ms);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
