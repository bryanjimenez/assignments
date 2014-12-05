import java.util.ArrayList;
import java.util.Arrays;

public class Square implements Comparable<Square> {
	private static final int INFINITY = Integer.MAX_VALUE;

	private static int[][] myValues;
	private static int[][] myDistances;
	private static int rows, cols;

	private int row, col;
	public Square previous;

	public ArrayList<Square> getNeighbors() {
		ArrayList<Square> neighbors = new ArrayList<Square>();

		// TL
		if (this.row > 0 && this.col > 0) {
			Square i = new Square(null, row - 1, col - 1);
			i.previous=this;
			neighbors.add(i);
		}
		// L
		if (this.col > 0) {
			Square i = new Square(null, row, col - 1);
			i.previous=this;
			neighbors.add(i);
		}
		// BL
		if (this.row < rows && this.col > 0) {
			Square i = new Square(null, row + 1, col - 1);
			i.previous=this;
			neighbors.add(i);
		}
		// B
		if (this.row < rows) {
			Square i = new Square(null, row + 1, col);
			i.previous=this;
			neighbors.add(i);
		}
		// BR
		if (this.row < rows && this.col < cols) {
			Square i = new Square(null, row + 1, col + 1);
			i.previous=this;
			neighbors.add(i);
		}
		// R
		if (this.col < cols) {
			Square i = new Square(null, row, col + 1);
			i.previous=this;
			neighbors.add(i);
		}
		// TR
		if (this.row > 0 && this.col < cols) {
			Square i = new Square(null, row - 1, col + 1);
			i.previous=this;
			neighbors.add(i);
		}
		// T
		if (this.row > 0) {
			Square i = new Square(null, row - 1, col);
			i.previous=this;
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
		return "("+this.row + "," + this.col+") cell is "+this.getValue();
	}
	public int getValue() {
		return myValues[row][col];
	}

	public int getDistance() {
		// if(myDistances[row][col]==null)

		return myDistances[row][col];
	}

	public void setDistance(int x) {
		if (myDistances[row][col] > x + this.getValue())
			myDistances[row][col] = x + this.getValue();
	}

	public int compareTo(Square s) {

		long theirs = ((Square) s).getDistance();
		long mine = this.getDistance();

		if (mine - theirs == 0 && !this.equals(s))
			return 1;
		else
			return (int) (mine - theirs);
	}

	/*
	 * @param
	 */
	public Square(int[][] z, int row, int col) {
		if (myValues == null) {
			myValues = z;
			cols = z[0].length - 1;
			rows = z.length - 1;

			myDistances = new int[rows + 1][cols + 1];
			for (int i = 0; i < rows + 1; i++)
				for (int j = 0; j < cols + 1; j++)
					myDistances[i][j] = INFINITY;
			// Arrays.fill(myDistances, -1);
			myDistances[0][0] = 0;
			this.previous=null;
		}

		this.row = row;
		this.col = col;
		

	}
}
