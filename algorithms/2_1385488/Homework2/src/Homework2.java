/*
 * 
 * @author Bryan Jimenez
 */
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class Homework2 {

	/*
	 * 
	 * change parameters here
	 */
	private static final String thematrix = "TheMatrix.txt";
	private static final String twolines = "TwoLines.txt";
	private static int change=100;
	private static int[] coinset ={ 1, 5, 10, 18, 25 };

	
	
	public static void getMaxSubMatrix(int[][] bigM) {
		int i, j, largest = 0, max_x = 0, max_y = 0, count = 0;
		int row = bigM.length;
		int col = bigM[0].length;
		int[][] helper = new int[row][col];

		// fill first row
		for (i = 0; i < row; i++)
			helper[i][0] = bigM[i][0];

		// fill first column
		for (i = 0; i < col; i++)
			helper[0][i] = bigM[0][i];

		// calculate the rest
		for (i = 1; i < row; i++) {
			for (j = 1; j < col; j++) {
				if (bigM[i][j] == 1) {
					// out of the three corners find the smallest and increment
					// by 1
					// same as getting the highest if no match
					// or adding one to far corner if match
					helper[i][j] = Math.min(
							Math.min(helper[i][j - 1], helper[i - 1][j]),
							helper[i - 1][j - 1]) + 1;
				} else
					helper[i][j] = 0;
			}
		}
		// find the largest

		for (i = 0; i < row; i++) {
			for (j = 0; j < col; j++) {
				// largest =(largest < helper[i][j]) ? helper[i][j] : largest;
				if (largest < helper[i][j]) {
					largest = helper[i][j];
					max_x = i;
					max_y = j;
					count = 0;
				}
				// count how many other equal size max are out there }
				else if (largest == helper[i][j])
					count++;
			}
		}
		// print helper
		/*
		 * for (i = 0; i < row; i++) { for (j = 0; j < col; j++) {
		 * System.out.print(helper[i][j]); } System.out.println(); }
		 */
		System.out.println("largest square: " + largest + "x" + largest);
		System.out.println("top left (" + (max_x - largest + 1) + ","
				+ (max_y - largest + 1) + ")");
		System.out.println("bottom right (" + max_x + "," + max_y + ")");
		System.out.println("found " + count + " other " + largest + "x"
				+ largest + "'s");

	}

	private static String lcs(String a, String b) {

		int[][] helper = new int[b.length() + 1][a.length() + 1];
		String answer = "", reversed = "";
		// row 0 and column 0 are initialized to 0 already

		for (int i = 1; i < b.length() + 1; i++)
			for (int j = 1; j < a.length() + 1; j++)
				if (a.charAt(j - 1) == b.charAt(i - 1))
					helper[i][j] = helper[i - 1][j - 1] + 1;
				else
					helper[i][j] = Math.max(helper[i - 1][j], helper[i][j - 1]);

		/*
		 * // print helper System.out.println("  "+a);
		 * 
		 * for (int i = 0; i < b.length()+1; i++){ if(i!=0)
		 * System.out.print(b.charAt(i-1)); else System.out.print(" ");
		 * 
		 * for (int j = 0; j < a.length()+1; j++)
		 * System.out.print(helper[i][j]); System.out.println(); }
		 */
		int i = b.length();
		int j = a.length();

		// retracing back from the bottom right
		while (i > 0 && j > 0)
			// System.out.print(i);
			// go up
			if (helper[i][j] == helper[i][j - 1]) {
				j--;
				// System.out.print("L");
			} else if (helper[i][j] == helper[i - 1][j]) {
				i--;
				// System.out.print("U");
			} else {
				// System.out.println("B"+b.charAt(i-1));
				reversed += b.charAt(i - 1);
				i--;
				j--;
			}

		for (i = reversed.length() - 1; i > -1; i--)
			answer += reversed.charAt(i);

		return answer;

	}

	private static void optimalChange(int sum, int[] coins) {

		// int sum = 100;
		int change;
		// int[] coins = { 1, 5, 10, 18, 25 };
		int[] numcoins = new int[sum + 1];
		int[] maxcoins = new int[sum + 1];

		// ignore the 0th place for niceness
		for (int i = 1; i < sum + 1; i++) {
			// start off with highest so compare search works
			numcoins[i] = Integer.MAX_VALUE;

			// just for readability
			change = i;

			// check if change is a one piece coin
			for (int j = 0; j < coins.length; j++)
				if (change == coins[j]) {
					numcoins[i] = 1;
					maxcoins[i] = coins[j];
					break;
				}
			// if its a one piece coin this will not do better
			// if not then search the previous rows for the best change (least
			// coins)
			for (int k = change; k > 0; --k) {
				if (numcoins[i] > numcoins[k] + numcoins[change - k]) {
					numcoins[i] = numcoins[k] + numcoins[change - k];
					maxcoins[i] = maxcoins[k];
				}
			}
			System.out
					.println((change) + " " + numcoins[i] + " " + maxcoins[i]);
		}
	}

	public static void main(String args[]) {
		try {
			// reading the matrix
			BufferedReader br = new BufferedReader(new FileReader(thematrix));
			int i;
			int x = 0, y = 0;
			int row = 0, col = 0;

			col = br.readLine().length();

			while (br.readLine() != null)
				row++;
			br.close();

			br = new BufferedReader(new FileReader(thematrix));

			int[][] input = new int[++row][col];

			while ((i = br.read()) != -1) {
				if (i == 48 || i == 49) {
					// System.out.print((i == 49) ? 1 : 0);
					input[x][y] = (i == 49) ? 1 : 0;
					y++;
					if (y == col) {
						y = 0;
						x++;
					}
				}
			}
			br.close();

			// reading two lines
			br = new BufferedReader(new FileReader(twolines));
			String a = br.readLine();
			String b = br.readLine();
			br.close();

			// calculate largest submatrix
			System.out.println("Largest Submatrix");
			// getMaxSubMatrix(new int[][] { { 1, 0, 1 }, { 1, 1, 1, },
			// { 1, 1, 1 },{ 1, 0, 1 },{ 1, 1, 1 } });
			getMaxSubMatrix(input);
			System.out.println("\n");

			// calculate largest common subsequence

			System.out.println("Largest Common Subsequence");
			String lcsStr = lcs(a, b);
			System.out.println("LCS: " + lcsStr);
			System.out.println("length: " + lcsStr.length() + "\n");

			// calculate optimal change

			System.out.println("Optimal Change");
			optimalChange(change,coinset);
			System.out.println("\n");

		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
