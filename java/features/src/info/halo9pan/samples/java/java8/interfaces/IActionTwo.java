package info.halo9pan.samples.java.java8.interfaces;

public interface IActionTwo {

	void run();

	default void init() {
		System.out.println("interface two method.");
	}
	
}
