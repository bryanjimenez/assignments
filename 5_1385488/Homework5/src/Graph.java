// Aknowledgements:
// http://algs4.cs.princeton.edu/64maxflow/

import java.util.HashMap;
import java.util.LinkedList;

public class Graph {
	private final int V;
	private int E;
	private LinkedList<Edge>[] adj;

	// empty graph with V vertices
	public Graph(int V) {
		this.V = V;
		this.E = 0;
		adj = (LinkedList<Edge>[]) new LinkedList[V];
		for (int v = 0; v < V; v++)
			adj[v] = new LinkedList<Edge>();
	}

	// number of vertices and edges
	public int V() {
		return V;
	}

	public int E() {
		return E;
	}

	// add edge e in both v's and w's adjacency lists
	public void addEdge(Edge e) {
		E++;
		int v = e.from();
		int w = e.to();
		adj[v].add(e);
		adj[w].add(e);
	}

	// return list of edges incident to v
	public Iterable<Edge> adj(int v) {
		return adj[v];
	}

	// return list of all edges - excludes self loops
	public Iterable<Edge> edges() {
		LinkedList<Edge> list = new LinkedList<Edge>();
		for (int v = 0; v < V; v++)
			for (Edge e : adj(v)) {
				if (e.to() != v)
					list.add(e);
			}
		return list;
	}
    public String toString() {
        String NEWLINE = System.getProperty("line.separator");
        StringBuilder s = new StringBuilder();
        s.append(V + " " + E + NEWLINE);
        for (int v = 0; v < V; v++) {
            s.append(v + ":  ");
            for (Edge e : adj[v]) {
                if (e.to() != v) s.append(e + "  ");
            }
            s.append(NEWLINE);
        }
        return s.toString();
    }
    
	public String flowPath(HashMap<Integer, String> names) {

		String NEWLINE = System.getProperty("line.separator");
		StringBuilder s = new StringBuilder();
		String path = "";

		for (int v = 0; v < V; v++) {
			for (Edge e : adj[v]) {
				if (e.to() != v && e.flow() == 1 && e.to() < V - 2
						&& e.from() < V - 2) {
					path += names.get(e.from()) + ": matched to "
							+ names.get(e.to()) + " (rank " + e.rank() + ")\n";
				}
			}
		}
		return path;
	}
}