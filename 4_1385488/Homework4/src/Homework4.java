import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.PriorityQueue;

/*
 * 
 * @author Bryan Jimenez
 */
public class Homework4 {
	private static String filename = "grid3.txt";
	private static final int INFINITY = Integer.MAX_VALUE;

	private static int[][] getInput(String filename) {
		int row = 0, col = 0;

		int[][] input = null;

		try {
			// reading the matrix
			BufferedReader br = new BufferedReader(new FileReader(filename));
			int i;
			int x = 0, y = 0;

			// Avoid the starting space
			col = br.readLine().split("[0-9] ").length;
			row++;

			while (br.readLine() != null)
				row++;
			br.close();

			// System.out.println(row+ " "+col);

			br = new BufferedReader(new FileReader(filename));

			input = new int[row][col];

			String s = "";

			while ((i = br.read()) != -1) {

				// space is the delimiter, the range is digits (0-9)
				if (i > 47 && i < 58) {
					s += i - 48;

				} else {
					if (s != "") {
						// System.out.print(s+" ");
						input[x][y] = Integer.parseInt(s);
						s = "";
						y++;
					}
					if (i != 32) {
						x++;
						y = 0;
					}
				}
			}
			br.close();
			/*
			 * for (x = 0; x < row; x++) { for (y = 0; y < col; y++) {
			 * System.out.print(input[x][y] + " "); } System.out.println(); }
			 */
		} catch (IOException e) {
			e.printStackTrace();
		}
		return input;
	}

	static private void part1(int[][] input) {

		// initialize all distances to INFINITY, except start distance is 0
		// pq is a priority queue that returns MINIMUM
		PriorityQueue<Square> pq = new PriorityQueue<>();
		pq.add(new Square(input, 0, 0));
		Square zero = null;
		while (!pq.isEmpty()) {
			Square v = pq.remove();
			// System.out.println(v.getValue());

			/*
			 * for each square w adjacent to v if w's distance is INFINITY set
			 * w's distance to v's distance + w's square cost, and add w to
			 * priority queue
			 */
			for (Square w : v.getNeighbors()) {
				if (w.getDistance() == INFINITY) {
					w.setDistance(v.getDistance());
					// System.out.println(w.getValue() + " " + w.getDistance());
					pq.add(w);
					if (w.getValue() == 0)
						zero = w;
				} else
					continue;
			}
		}
		ArrayList<String> path = new ArrayList<>();
		int distance = zero.getDistance();
		while (zero != null) {
			path.add(zero.info());
			zero = zero.previous;
		}
		Collections.reverse(path);

		if (path.size() > 20) {
			for (int i = 0; i < 10; i++)
				System.out.println(path.get(i));

			System.out.println("...");

			for (int i = path.size() - 10; i < path.size(); i++)
				System.out.println(path.get(i));

		} else {
			for (String x : path)
				System.out.println(x);
		}

		System.out.println("Total cost is " + distance);
	}

	static private void part2(int[][] input) {

		/*
		 * initialize all weights to 0, except adjust start and end square to
		 * INFINITY pq is a priority queue that returns MAXIMUM
		 */
		PriorityQueue<Square2> pq = new PriorityQueue<>();
		pq.add(new Square2(input, 0, 0));
		Square2 zero = null;
		while (!pq.isEmpty()) {
			Square2 v = pq.remove();

			// System.out.println(v.getValue());

			/*
			 * for each square w adjacent to v if w's distance is INFINITY set
			 * w's distance to v's distance + w's square cost, and add w to
			 * priority queue
			 */
			for (Square2 w : v.getNeighbors()) {
				if (w.getDistance() == 0) {
					w.setDistance(v.getDistance());
					// System.out.println(w.getValue() + " " + w.getDistance());
					pq.add(w);

				} else if (w.getDistance() == INFINITY
						&& w.getValue() == INFINITY) {
					w.setDistance(v.getDistance());
					// System.out.println(w.getValue() + "!"
					// +w.previous.getValue());
					w.fix();
					zero = w;
					pq.clear();
					break;
				} else
					continue;
			}
		}
		ArrayList<String> path = new ArrayList<>();
		int distance = Integer.MAX_VALUE;
		while (zero != null) {
			path.add(zero.info());
			if (zero.getDistance() != 0)
				distance = Math.min(distance, zero.getDistance());
			zero = zero.previous;
		}
		Collections.reverse(path);

		if (path.size() > 20) {
			for (int i = 0; i < 10; i++)
				System.out.println(path.get(i));

			System.out.println("...");

			for (int i = path.size() - 10; i < path.size(); i++)
				System.out.println(path.get(i));

		} else {
			for (String x : path)
				System.out.println(x);
		}

		System.out.println("All cells support " + distance);
	}

	static public void main(String args[]) {

		int[][] input = getInput(filename);

		part1(input);

		System.out.println();

		part2(input);

	}
}
