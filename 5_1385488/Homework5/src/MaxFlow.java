// Aknowledgements:
// http://algs4.cs.princeton.edu/64maxflow/

import java.util.LinkedList;
import java.util.Queue;

public class MaxFlow {
	private boolean[] marked; // marked[v] = true iff s->v path in residual
								// graph
	private Edge[] edgeTo; // edgeTo[v] = last edge on shortest residual s->v
							// path
	private int value; // current value of max flow

	// max flow in flow network G from s to t
	public MaxFlow(Graph G, int s, int t) {

		// while there exists an augmenting path, use it
		while (hasAugmentingPath(G, s, t)) {
			for (int v = t; v != s; v = edgeTo[v].other(v)) {
				edgeTo[v].addResidualFlowTo(v);
			}
			value++;
		}
	}

	// return value of max flow
	public int value() {
		return value;
	}

	// is there an augmenting path?
	// if so, upon termination edgeTo[] will contain a parent-link
	// representation of such a path
	private boolean hasAugmentingPath(Graph G, int s, int t) {
		edgeTo = new Edge[G.V()];
		marked = new boolean[G.V()];

		// breadth-first search
		Queue<Integer> q = new LinkedList<Integer>();
		q.add(s);
		marked[s] = true;
		while (!q.isEmpty()) {
			int v = q.remove();

			for (Edge e : G.adj(v)) {
				int w = e.other(v);

				// if residual capacity from v to w
				if (e.residualCapacityTo(w) > 0) {
					if (!marked[w]) {
						edgeTo[w] = e;
						marked[w] = true;
						q.add(w);
					}
				}
			}
		}

		// is there an augmenting path?
		return marked[t];
	}
}