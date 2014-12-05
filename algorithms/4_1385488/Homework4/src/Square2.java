import java.util.ArrayList;
import java.util.Arrays;

public class Square2 implements Comparable<Square2> {
	private static final int INFINITY = Integer.MAX_VALUE;

	private static int[][] myValues;
	private static int[][] myDistances;
	private static int rows, cols;

	private int row, col;
	public Square2 previous;

	public ArrayList<Square2> getNeighbors() {
		ArrayList<Square2> neighbors = new ArrayList<Square2>();

		// TL
		if (this.row > 0 && this.col > 0) {
			Square2 i = new Square2(null, row - 1, col - 1);
			i.previous = this;
			neighbors.add(i);
		}
		// L
		if (this.col > 0) {
			Square2 i = new Square2(null, row, col - 1);
			i.previous = this;
			neighbors.add(i);
		}
		// BL
		if (this.row < rows && this.col > 0) {
			Square2 i = new Square2(null, row + 1, col - 1);
			i.previous = this;
			neighbors.add(i);
		}
		// B
		if (this.row < rows) {
			Square2 i = new Square2(null, row + 1, col);
			i.previous = this;
			neighbors.add(i);
		}
		// BR
		if (this.row < rows && this.col < cols) {
			Square2 i = new Square2(null, row + 1, col + 1);
			i.previous = this;
			neighbors.add(i);
		}
		// R
		if (this.col < cols) {
			Square2 i = new Square2(null, row, col + 1);
			i.previous = this;
			neighbors.add(i);
		}
		// TR
		if (this.row > 0 && this.col < cols) {
			Square2 i = new Square2(null, row - 1, col + 1);
			i.previous = this;
			neighbors.add(i);
		}
		// T
		if (this.row > 0) {
			Square2 i = new Square2(null, row - 1, col);
			i.previous = this;
			neighbors.add(i);
		}

		return neighbors;
	}

	public void size() {
		// y by x
		System.out.println(this.rows + " " + this.cols);
	}

	public String info() {
		// y by x
		String val;

		if (this.row == 0 && this.col == 0)
			val = "START";
		else if (this.row == rows && this.col == cols)
			val = "END";
		else
			val = "" + this.getValue();
		return "(" + this.row + "," + this.col + ") cell is " + val;
	}

	public int getValue() {
		return myValues[row][col];
	}

	public int getDistance() {
		// if(myDistances[row][col]==null)

		return myDistances[row][col];
	}

	public void setDistance(int x) {
		// if(this.getValue()!=INFINITY)
		myDistances[row][col] = Math.min(x, this.getValue());
	}

	public void fix() {
		myValues[rows][cols] = 0;
	}

	public int compareTo(Square2 s) {

		long theirs = ((Square2) s).getDistance();
		long mine = this.getDistance();

		if (theirs - mine == 0 && !this.equals(s))
			return 1;
		else
			return (int) (theirs - mine);
	}

	/*
	 * @param
	 */
	public Square2(int[][] z, int row, int col) {
		if (myValues == null) {
			myValues = z;
			cols = z[0].length - 1;
			rows = z.length - 1;

			myDistances = new int[rows + 1][cols + 1];
			for (int i = 0; i < rows + 1; i++)
				for (int j = 0; j < cols + 1; j++)
					myDistances[i][j] = 0;
			myDistances[0][0] = INFINITY;
			myDistances[rows][cols] = INFINITY;
			myValues[rows][cols] = INFINITY;
			this.previous = null;
		}

		this.row = row;
		this.col = col;

	}
}
