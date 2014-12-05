import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.ArrayList;
import java.io.IOException;

public class Homework6 {
	private static String filename = "flips4.txt";

	private static String getInput(ArrayList<Integer> lines,
			ArrayList<Integer> omits, StringBuilder original) {
		StringBuilder output =new StringBuilder();

		int i = 0, c = 0;
		int consecutive = 0;

		
		try {
			BufferedReader br = new BufferedReader(new FileReader(filename));

			while ((i = br.read()) != -1) {
				c++;
				original.append(Character.toString((char) (i)));

				if (i >= 65 && i <= 90) { // A-Z
					output.append(Character.toString((char) (i + 32)));
				} else if (i >= 97 && i <= 122) { // a-z
					output.append(Character.toString((char) (i )));
				} else if (i >= 48 && i <= 57) { // 0-9
					output.append(Character.toString((char) (i)));
				} else if (i == 10) { // new line
					lines.add(c);
					omits.add(c - consecutive - 1);
					consecutive++;

				} else { // everything else
					omits.add(c - consecutive - 1);
					consecutive++;

					// System.out.println(Character.toString ((char) i)+" "+i);
				}

			}
			br.close();

		} catch (IOException e) {
			e.printStackTrace();
		}

		return output.toString();
	}

	public static void main(String[] args) throws FileNotFoundException {
		ArrayList<Integer> lines = new ArrayList<Integer>();
		ArrayList<Integer> omits = new ArrayList<Integer>();
		
		
		StringBuilder ostr = new StringBuilder("");
		String str = getInput(lines, omits, ostr);


		int[] sa = new int[str.length()];
		int[] LCP = new int[str.length()];

		int[] LAST = new int[str.length()];
		int[] CURR = new int[str.length()];
		int[] BEST = new int[str.length()];

		// will store index where the position is
		// found to get the starting index of the substring
		int[] pos = new int[str.length()];

		// SuffixArray.test(str);
		SuffixArray.createSuffixArray(str, sa, LCP);

		int K = 2;
		int last = 0;

		for (int i = 2; i < 80; i++) {
			last = Math.min(LCP[i], LCP[i - 1]);

			// Get the best pair
			if (BEST[2] < LCP[i]) {
				BEST[2] = LCP[i];
				pos[2] = i;
			}
			// BEST[2]=BEST[2]<LCP[i]?LCP[i]:BEST[2];

			if (last > 0) {
				K++;

				LAST[2] = LCP[i - 1];
				CURR[2] = LCP[i];

				// System.out.println("i:"+i+" K:"+K);
				// System.out.print("LAST:");

				// LAST
				// for(int k=3;k<K+1;k++){
				// System.out.print(LAST[k]);
				// }
				// System.out.println();

				// CURRENT
				// System.out.print("CURR:");
				for (int k = 3; k < K + 1; k++) {
					// System.out.println(CURR[w-1]+","+LAST[w-1]);

					CURR[k] = Math.min(CURR[k - 1], LAST[k - 1]);
					// System.out.print(CURR[k]);
				}
				// System.out.println();

				// System.out.print("BEST:");

				// Copy only until end of CURR
				for (int k = 3; CURR[k] > 0; k++) {
					if (BEST[k] < CURR[k]) {
						BEST[k] = CURR[k];
						pos[k] = i;
					}
					// System.out.print(BEST[k]);
				}
				// System.out.println();

				// Rotate CURR to LAST
				for (int k = 3; k < K + 1; k++) {
					LAST[k] = CURR[k];
				}
			} else {
				K = 2;
				LAST = new int[str.length()];
				CURR = new int[str.length()];
			}
		}

		int i = 2;

		int line = 0;
		int offset1 = 0;
		int offset2 = 0;

		// for(int e:omits)
		// System.out.println(e);

		while (BEST[i] > 0) {
			System.out.println("Longest sequence that occurs " + i
					+ " times has basic length " + BEST[i] + " and is is "
					+ str.substring(sa[pos[i]], sa[pos[i]] + BEST[i]));
			for (int j = 0; j < i; j++) {

				while (omits.get(offset1) <= sa[pos[i] - j])
					offset1++;

				while (omits.get(offset2) < sa[pos[i] - j] + BEST[i])
					offset2++;

				while (lines.get(line) < sa[pos[i] - j] + offset1)
					line++;

				System.out.println("Line "
						+ (line + 1)
						+ ": "
						+ ostr.substring(sa[pos[i] - j] + offset1, sa[pos[i]
								- j]
								+ BEST[i] + offset2));

//				 System.out.println(sa[pos[i] - j]+"+"+offset1+" "+(sa[pos[i]
//				 - j] + BEST[i])+"+"+offset2);

				line = 0;
				offset1 = 0;
				offset2 = 0;
			}
			i++;
		}
	}
}
