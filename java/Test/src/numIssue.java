public class numIssue {

	public static float getNumber(String eq) {
		float[] stackArray = new float[100];
		int stackPointer = 0;

		for (int i = 0; i < eq.length(); i++) {
			char c = eq.charAt(i);
			if ((c >= '0') && (c <= '9')) {
				stackArray[stackPointer] = stackArray[stackPointer] * 10
						+ (c - '0');
			}
			if (c == '+') {
				stackPointer--;
				stackArray[stackPointer] = stackArray[stackPointer]
						+ stackArray[stackPointer + 1];
			}
			if (c == '-') {
				stackPointer--;
				stackArray[stackPointer] = stackArray[stackPointer]
						- stackArray[stackPointer + 1];
			}
			if (c == '*') {
				stackPointer--;
				stackArray[stackPointer] = stackArray[stackPointer]
						* stackArray[stackPointer + 1];
			}
			if (c == '/') {
				stackPointer--;
				stackArray[stackPointer] = stackArray[stackPointer]
						/ stackArray[stackPointer + 1];
			}
			if (c == ' ') {
				stackPointer++;
				stackArray[stackPointer] = 0;
			}
		}

		return stackArray[0];
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		//System.out.println(getNumber("4 6/"));

		String st1 = "1346";

		String dd = "+-*/";

		for (int l = 0; l < st1.length(); l++)
			for (int m = 0; m < st1.length(); m++)
				if (m != l)
					for (int n = 0; n < st1.length(); n++)
						if ((n != l) && (n != m))
							for (int o = 0; o < st1.length(); o++)
								if ((o != l) && (o != m) && (o != n)) {
									String sss = String.format("%c %c %c %c",
											st1.charAt(l), st1.charAt(m),
											st1.charAt(n), st1.charAt(o));
									for (int i = 0; i < dd.length(); i++)
										for (int j = 0; j < dd.length(); j++)
											for (int k = 0; k < dd.length(); k++) {
												String eq = sss + dd.charAt(i)
														+ dd.charAt(j)
														+ dd.charAt(k);
												float r = getNumber(eq);
												if ((r > 9.99) && (r < 10.01))
													System.out.println(eq + " "
															+ r);
											}
								}
	}

}