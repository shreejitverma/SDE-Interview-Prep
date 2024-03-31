/* Checking runtime polymorphism as claimed in Java.
 *
 */

package problem_solving;

class BaseClass {
	void show() {
		System.out.println("Base class show()");
	}
}

class ChildClass extends BaseClass{
	void show() {
		System.out.println("Child class show()");
	}
	void showMore() {
		System.out.println("Child class showMore()");	
	}
}

public class MakeObject {
	public static void main(String[] args) {
	BaseClass p = new ChildClass();
	p.show();
	/* Parent reference type p is not able to reference 
	 * a specific method of Child class.
	 */
	//p.showMore();
	p = new BaseClass();
	p.show();
}
}