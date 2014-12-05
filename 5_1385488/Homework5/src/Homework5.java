import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;

public class Homework5 {
	private static String filename = "friends8.txt";

	private static HashMap<String, Integer> names = new HashMap<String, Integer>();
	private static HashMap<Integer, String> numbers = new HashMap<Integer, String>();

	private static Graph[][] getInput(String filename) {
		Graph G[][] = null;

		try {
			BufferedReader br = new BufferedReader(new FileReader(filename));

			String s;
			int i = 0;
			int N;
			s = br.readLine();

			N = s.split(",").length;
			G = new Graph[2][N];

			// Create the 0-N networks
			for (int o = 0; o < N; o++) {
				G[0][o] = new Graph(2 * N + 2);
				G[1][o] = new Graph(2 * N + 2);
			}
			// get female names
			for (String name : s.split(":")[1].split(",")) {
				names.put(name, i);
				numbers.put(i++, name);
			}

			// get male names
			names.put(s.split(":")[0], i);
			numbers.put(i++, s.split(":")[0]);

			while ((s = br.readLine()) != null) {
				if (s.split(",").length > 1) {
					names.put(s.split(":")[0], i);
					numbers.put(i++, s.split(":")[0]);
				} else
					break;
			}
			br.close();

			br = new BufferedReader(new FileReader(filename));

			i = 0;
			int L = 0;
			while ((s = br.readLine()) != null) {
				if (s.indexOf(":") < 0)
					continue;
				if (i >= N) {
					String v = s.split(":")[0];
					int r = 0;
					for (String w : s.split(":")[1].split(",")) {
							G[0][r++].addEdge(new Edge(names.get(v),
									names.get(w), 1, r));
					}
				} else if (i < N) {
					String v = s.split(":")[0];
					int r = 0;
					for (String w : s.split(":")[1].split(",")) {
							G[1][r++].addEdge(new Edge(names.get(v),
									names.get(w), 1, r));
					}
				}
				i++;
			}
			br.close();
			//System.out.println(G[1][0].toString());
			//System.out.println(G[1][1].toString());

		} catch (IOException e) {
			e.printStackTrace();
		}
		return G;
	}

	private static int binarySearch(int key, Graph[][] G, int start, int end) {

		int k = (end - start + 1) / 2 + start; 

		// System.out.println(k);

		int s = G[0][0].V() - 2;
		int t = G[0][0].V() - 1;

		MaxFlow maxflowF = new MaxFlow(G[0][k], s, t);
		MaxFlow maxflowM = new MaxFlow(G[1][k], s, t);

		int males = maxflowM.value();
		int females = maxflowF.value();

		maxflowF = null;
		maxflowM = null;

		System.out.print(k + " " + males + " " + females + " " + start + " "
				+ end);

		if (key == males && key == females) {
			if (end - start == 1 || end == start) {
				System.out.println("done");
				return end;
			}
			System.out.println("low");
			return binarySearch(key, G, start, k); // recurse lower half
		}
		if (key > males || key > females) {
			System.out.println("high");
			return binarySearch(key, G, k + 1, end); // recurse higher half
		}

		return -1;
	}

	public static void main(String[] args) {

		Graph G[][] = getInput(filename);

		int N = (G[0][0].V() - 2) / 2;
		int s = G[0][0].V() - 2;
		int t = G[0][0].V() - 1;

		// Add s and t for all networks
			for (int i = 0; i < N; i++) {
				G[0][0].addEdge(new Edge(s, i, 1,0));
				G[0][0].addEdge(new Edge(i + N, t, 1, 0));
				G[1][0].addEdge(new Edge(s, i + N, 1, 0));
				G[1][0].addEdge(new Edge(i, t, 1, 0));
			}

			
		long time = System.currentTimeMillis();
		
		
		// LINEAR SEARCH
		int k=0;		
		int males = 0,females=0;
		int match;
		
		while(k<N){
			 MaxFlow m = new MaxFlow(G[1][0], s, t);
			 MaxFlow f = new MaxFlow(G[0][0], s, t);
			 males += m.value();
			 females += f.value();
			
//			 match=0;
//			 for(Edge e:G[1][0].edges()){
//				 for(Edge x:G[0][k].adj(e.to())){
			 
			 		//if theres an outgoing edge from male to female where the female also has a back edge
//					 if(x.to()==e.from()&&e.flow()==1){	
//						 System.out.print("e"+match+e.toString());
//						 System.out.println("\tx"+match+x.toString());
//						match++;
//					}
//				 }
//			 }
//			 System.out.println("match"+match);
			 
			//	System.out.print(G[1][0].toString());
			//	System.out.print(G[0][0].toString());
			 if (males == N)
			//if ((males == N)&&(females==N))
				break;
			else{
				for(Edge e:G[1][k+1].edges())
					G[1][0].addEdge(e);
				for(Edge e:G[0][k+1].edges())
					G[0][0].addEdge(e);
			}
			k++;
		}
				
				
		System.out.println("Everybody matched with top " + (k + 1)
				+ " preferences:");


		time = System.currentTimeMillis() - time;
		System.out.println("Elapsed time: " + time + " ms");

		System.out.print(G[1][0].flowPath(numbers));
		System.out.print(G[0][0].flowPath(numbers));
		
		//Not working correctly
		
		// Binary Search
		// time = System.currentTimeMillis();
		// k = binarySearch(N, G, 0, N - 1);
		// time = System.currentTimeMillis() - time;
		// System.out.println("Elapsed time: " + time + " ms");
		//
		// System.out.println("Everybody matched with top " + (k + 1)
		// + " preferences:");
		// System.out.print(G[1][k].flowPath(numbers));
		// System.out.println(G[0][k].flowPath(numbers));

	}
}