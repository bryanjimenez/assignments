import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.NavigableSet;
import java.util.TreeSet;

/*
 * 
 * @author Bryan Jimenez
 */

//http://www.cs.princeton.edu/courses/archive/fall07/cos226/assignments/bins.html
//http://www.cs.ucsb.edu/~suri/cs130b/BinPacking.txt
//http://www.tutorialspoint.com/java/util/treeset_headset.htm

public class Homework3 {

	//MODIFY THESE PARAMETERS
	private static long BIN_MAX = 1000000000;
	private static String input = "data6.txt";
	
	
	static Collection<Bin> nextFit(LinkedList<Integer> x) {
		ArrayList<Bin> bins = new ArrayList<Bin>();

		for (int i : x) {
			
			//if the bin list is empty or it doesnt fit in the previous then create a new bin
			if (bins.isEmpty()
					|| bins.get(bins.size() - 1).getWeight() > (BIN_MAX - i)) {
				bins.add(new Bin(i));
			} else {
				//insert in previous bin if it fits
				bins.get(bins.size() - 1).addNewItem(i);
			}
		}
		return bins;
	}

	static NavigableSet<Bin> firstFit(List<Integer> x) {
		NavigableSet<Bin> bins = new TreeSet<Bin>();
		NavigableSet<Bin> mins = new TreeSet<Bin>();
		NavigableSet<Bin> head = new TreeSet<Bin>();
		boolean found;
		Bin nextmin;
		
		for (Integer i : x) {
			found = false;

			head = mins.headSet(new Bin((int) (BIN_MAX - i)), true);
			Iterator<Bin> mini = head.descendingIterator();

			while (mini.hasNext()) {
				nextmin = mini.next();

				// it fits in the min bin
				if (nextmin != null && BIN_MAX - nextmin.getWeight() >= i) {
					// System.out.print("+"+i);
					bins.remove(nextmin);
					mins.remove(nextmin);
					nextmin.addNewItem(i);

					// check its not larger than the previous min (should be
					// still smaller to stay in min)
					if (mins.isEmpty())
						mins.add(nextmin);
					else if (mins.first().getWeight() >= nextmin.getWeight())
						mins.add(nextmin);

					bins.add(nextmin);
					found = true;
					break;
				}
			}

			head = null;

			if (!found) {
				Bin nuevo = new Bin(i);
				bins.add(nuevo);
				if (mins.isEmpty() || mins.first().getWeight() > i)
					mins.add(nuevo);
			}
		}
		return bins;
	}

	static NavigableSet<Bin> bestFit(List<Integer> x) {
		NavigableSet<Bin> bins = new TreeSet<Bin>();
		NavigableSet<Bin> head = new TreeSet<Bin>();
		boolean found;
		Bin nextbin;
		
		for (Integer i : x) {
			found = false;
			head = bins.headSet(new Bin((int) (BIN_MAX - i)), true);
			Iterator<Bin> iter = head.descendingIterator();

			while (iter.hasNext()) {
				nextbin = iter.next();

				if (nextbin != null && BIN_MAX - nextbin.getWeight() >= i) {
					bins.remove(nextbin);
					nextbin.addNewItem(i);
					bins.add(nextbin);
					found = true;
					break;
				}
			}
			head = null;

			if (!found)
				bins.add(new Bin(i));
		}
		return bins;
	}

	static NavigableSet<Bin> worstFit(List<Integer> x) {
		NavigableSet<Bin> bins = new TreeSet<Bin>();
		NavigableSet<Bin> head = new TreeSet<Bin>();
		boolean found;
		Bin nextbin;
		
		for (Integer i : x) {
			found = false;
			head = bins.headSet(new Bin((int) (BIN_MAX - i)), true);
			Iterator<Bin> iter = head.iterator();

			while (iter.hasNext()) {
				nextbin = iter.next();

				if (nextbin != null && BIN_MAX - nextbin.getWeight() >= i) {
					bins.remove(nextbin);
					nextbin.addNewItem(i);
					bins.add(nextbin);
					found = true;
					break;
				}
			}
			head = null;

			if (!found)
				bins.add(new Bin(i));
		}
		return bins;
	}

	static void printStats(String name, long t, Collection<Bin> l) {
		long total = 0;
		int count = 0;
		for (Bin i : l) {
			total += i.getWeight();
			count++;
		}

		System.out.println(name);
		System.out.println(" time:\t" + t + " ms");
		System.out.println(" ideal:\t" + (total / BIN_MAX) + " bins");
		System.out.println(" used:\t" + count + " bins");
		System.out.print(" [0-9]:\t");
		count = 0;
		for (Bin i : l) {
			if (count == 10)
				break;
			System.out.print(i.getWeight() + " ");
			count++;
		}
		System.out.println();

	}

	public static void main(String args[]) {
		String s;
		BufferedReader br;
		LinkedList<Integer> c = new LinkedList<Integer>();
		long start, end, total;
		int count;

		try {
			br = new BufferedReader(new FileReader(input));

			while ((s = br.readLine()) != null)
				c.add(Integer.parseInt(s));

		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		start = System.currentTimeMillis();
		Collection<Bin> answer1 = nextFit(c);
		end = System.currentTimeMillis();
		printStats("nextFit", (end - start), answer1);
		answer1=null;

		start = System.currentTimeMillis();
		NavigableSet<Bin> answer2 = firstFit(c);
		end = System.currentTimeMillis();
		printStats("firstFit", (end - start), answer2);
		answer2=null;
		
		start = System.currentTimeMillis();
		NavigableSet<Bin> answer3 = bestFit(c);
		end = System.currentTimeMillis();
		printStats("bestFit", (end - start), answer3);
		answer3=null;
		
		start = System.currentTimeMillis();
		NavigableSet<Bin> answer4 = worstFit(c);
		end = System.currentTimeMillis();
		printStats("worstFit", (end - start), answer4);
		answer4=null;

		Collections.sort(c);
		//Collections.reverse(c);
		
//		for(int i : c)
//			System.out.print(" "+i);
		
		start = System.currentTimeMillis();
		Collection<Bin> answer1o = nextFit(c);
		end = System.currentTimeMillis();
		printStats("nextFit offline", (end - start), answer1o);
		answer1o=null;

		start = System.currentTimeMillis();
		NavigableSet<Bin> answer2o = firstFit(c);
		end = System.currentTimeMillis();
		printStats("firstFit offline", (end - start), answer2o);
		answer2o=null;

		start = System.currentTimeMillis();
		NavigableSet<Bin> answer3o = bestFit(c);
		end = System.currentTimeMillis();
		printStats("bestFit offline", (end - start), answer3o);
		answer3o=null;

		start = System.currentTimeMillis();
		NavigableSet<Bin> answer4o = worstFit(c);
		end = System.currentTimeMillis();
		printStats("worstFit offline", (end - start), answer4o);
		answer4o=null;
	}
}
