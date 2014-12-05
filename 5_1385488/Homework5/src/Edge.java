// Aknowledgements:
// http://algs4.cs.princeton.edu/64maxflow/


public class Edge {
	private final int v; // from
	private final int w; // to
	private final double capacity; // capacity
	private double flow; // flow
	private int rank; // rank

	public Edge(int v, int w, double capacity, int rank) {
		if (capacity < 0)
			throw new IllegalArgumentException("Negative edge capacity");
		this.v = v;
		this.w = w;
		this.capacity = capacity;
		this.flow = 0;
		this.rank = rank;
	}

	// accessor methods
	public int from() {
		return v;
	}

	public int to() {
		return w;
	}

	public double capacity() {
		return capacity;
	}

	public double flow() {
		return flow;
	}

	public int rank() {
		return rank;
	}

	public int other(int vertex) {
		if (vertex == v)
			return w;
		else if (vertex == w)
			return v;
		else
			throw new IllegalArgumentException("Illegal endpoint");
	}

	public double residualCapacityTo(int vertex) {
		if (vertex == v)
			return flow; // backward edge
		else if (vertex == w)
			return capacity - flow; // forward edge
		else
			throw new IllegalArgumentException("Illegal endpoint");
	}

	public void addResidualFlowTo(int vertex) {
		if (vertex == v)
			flow--; // backward edge
		else if (vertex == w)
			flow++; // forward edge
		else
			throw new IllegalArgumentException("Illegal endpoint");
	}
    public String toString() {
        return v + "->" + w + " " + flow + "/" + capacity;
    }
}