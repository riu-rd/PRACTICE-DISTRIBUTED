import java.util.Scanner;

public class App {
    
    public static void main(String[] args) throws Exception {
        
        // Main Query Loop
        Scanner scanner = new Scanner(System.in);
        Graph graph = new Graph("graph.txt");
        
        while (true) {
            System.out.print("Enter query: ");
            String query = scanner.nextLine();
            if (query.equals("exit")) {
                scanner.close();
                break;
            } else if (query.equals("clear")) {
                System.out.print("\033[H\033[2J");
                System.out.flush();
                continue;
            }
            graph.queryGraph(query);
        }
    }
}
