import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;


public class SimpleSound {

	
	public static void main(String[] args) {
		byte[] buf = new byte[ 11290 ];
	    AudioFormat af = new AudioFormat( (float )44100, 8, 1, true, false );
		try {
			SourceDataLine sdl = AudioSystem.getSourceDataLine( af );
		    sdl.open( af );
		    sdl.start();
		    for( int i = 0; i < 256 * (float )44100 / 1000; i++ ) {
		        double angle = i / ( (float )44100 / 1000 ) * 2.0 * Math.PI;
		        buf[ i ] = (byte )( Math.sin( angle ) * 100 );
		    }
	        sdl.write( buf, 0, 11290 );
		    sdl.drain();
		    sdl.stop();
		    sdl.close();
		} catch (LineUnavailableException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
}
