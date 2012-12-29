import javax.sound.sampled.*;


public class MusicGen {
	
	public static final float BUFFER_SIZE_SECONDS = (float) 1;
	public static final int CHANNELS = 1;
	public static final int BITS_PER_SAMPLE = 8;
	public static final int SAMPLE_RATE = 44100;
	public static final int BUFFER_SIZE_SAMPLES = (int) (SAMPLE_RATE * BUFFER_SIZE_SECONDS);
	public static final int NOTE_TIME_MS = 200;
	public static final float NOTE_A2 = (float) 110;
	
	byte bsina[] = {1,2,3};
	
	static int[] notes = {110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760}; 
	
	static int[] voice1 = {24, 24, 21, 21, 24, 24, 21, 21, 24, 22, 21, 19, 17, 17, 17, 17,
						   26, 26, 29, 26, 24, 24, 21, 21, 24, 22, 21, 19, 17, 17, 17, 17};
	
	static int[] voice2 = {5, 0, 0, 12, 9, 4, 4, 12, 7, 2, 0, 0, 5, 5, 0, 12,
						   10, 5, 5, 10, 9, 0, 0, 5, 7, 7, 0, 0, 5, 5, 17, 17};
	
	static char bsin (char i) {
		float a = (float) (((float)i / 128) * Math.PI);
		char result = (char)(Math.sin(a) * 127 + 128);
		return result;
	}
	
	static char bsqr (char i) {
		int x = (int) i;
		char result = (char) ((x<128)?1:254);
		return result;
	}	
	
	static char bswt (char i) {
		char result = (char) (i);
		return result;
	}
	
	static char brst (char i) {
		char result = (char) ((i*2)+((i*2>255)?(255-i*4):0));
		return result;
	}
	
	static char music (int t) {
		int noteN = (int) (t*1000/SAMPLE_RATE/NOTE_TIME_MS);
		int freq1 = notes[voice1[noteN % 32]];
		int freq2 = notes[voice2[noteN % 32]];
		char result = 0;
		
    	char x1 = (char) ((((float)t)*freq1*256/SAMPLE_RATE)%256);
    	char v1 = (char) (bsqr(x1) - 128);
    	char x2 = (char) ((((float)t)*freq2*256/SAMPLE_RATE)%256);
    	char v2 = (char) (bsqr(x2) - 128);
    	result = (char) ((v1+v2)/2);
		//System.out.printf("%d %d %d\n", (int)t, (int)(t*1000/SAMPLE_RATE), (int)noteN);
		return result;
	}
	
	public static void main(String[] args) {
		
		for (int i = 0; i<256; i++) {
			float nt = (float) (NOTE_A2 * Math.pow(2, i/12.0));
			int m = (int) (nt+0.5);
			//System.out.printf("%f, ", nt);
			System.out.printf("%d, ", m);
		}
		
		
		for (int i = 0; i<256; i++) {
			char c = (char) i;
			char r = bsin(c);
			char p = bsqr(c);
			//System.out.printf("0x%X 0x%X\n", (int)r, (int)p);
		}
		
		
		byte[] buf = new byte[BUFFER_SIZE_SAMPLES];
	    AudioFormat af = new AudioFormat((float)SAMPLE_RATE, BITS_PER_SAMPLE, CHANNELS, true, false);

		try {
			SourceDataLine sdl = AudioSystem.getSourceDataLine(af);
		    sdl.open(af);
		    sdl.start();
		    int j = 0;
		    for (int k = 0; k < 1000; k++) {
		    	for( int i = 0; i < BUFFER_SIZE_SAMPLES; i++ ) {
//		        double angle = i / ( (float )44100 / 1000 ) * 2.0 * Math.PI;
//		        buf[ i ] = (byte )( Math.sin( angle ) * 100 );
		    		buf[i] = (byte) music(i+j);
//		    	char x = (char) ((((float)i)*1000*256/SAMPLE_RATE)%256);
//		    	buf[i] = (byte) (bsin(x) - 128);
//		    	buf[i] = (byte) (bsqr(x) - 128);
//		    	buf[i] = (byte) (bswt(x) - 128);
//		    	System.out.println((int)x);
		    	}
		    	sdl.write( buf, 0, BUFFER_SIZE_SAMPLES );
		    	j = j + BUFFER_SIZE_SAMPLES;
		    }
		    sdl.drain();
		    sdl.stop();
		    sdl.close();
		} catch (LineUnavailableException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
