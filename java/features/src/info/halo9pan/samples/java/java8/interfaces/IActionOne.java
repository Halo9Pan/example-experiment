package info.halo9pan.samples.java.java8.interfaces;

public interface IActionOne {

	void run();

	default void init() {
		System.out.println("interface one method.");
	}
	
}
