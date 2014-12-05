import java.util.LinkedList;

public class Bin implements Comparable<Bin> {

	private LinkedList<Integer> items;

	public long getWeight() {
		long sum = 0;

		for (int x : this.items)
			sum += x;
		
		return sum;
	}

	public LinkedList<Integer> getAllItems() {
		return this.items;
	}

	public void addNewItem(int item) {
		this.items.add(item);
	}

	public int compareTo(Bin bin) {

		long q = ((Bin) bin).getWeight();
		long myweight = this.getWeight();
		
		// if not we loose bins with same size on inserts
		if (myweight - q == 0 && !this.equals(bin))
			return 1;
		else
			return (int)(myweight - q);
	}

	public Bin() {
		this.items = new LinkedList<Integer>();
	}

	public Bin(int x) {
		this.items = new LinkedList<Integer>();
		this.items.add(x);
	}
}
