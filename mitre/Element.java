import clientApi.*;

/**
 * The Element class (should be automatically generated by the java marshaling tool)
 *
 * @author Jeong-Hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 02/04/2005
 */
public class Element
    extends Tuple {

  public int timestamp;
  public int id;
  public int x;
  public int y;
  public int color;
  public int heading;

  public Element() {
    this.timestamp = 0;
    this.id = 0;
    this.x = 0;
    this.y = 0;
    this.color = 0;
    this.heading = 0;
  }

  public Element(int timestamp, int id, int x,
                 int y, int color, int heading) {
    this.timestamp = timestamp;
    this.id = id;
    this.x = x;
    this.y = y;
    this.color = color;
    this.heading = heading;
  }

  public String toString() {
    String s = "(";
    s += timestamp;
    s += ", ";
    s += id;
    s += ", ";
    s += x;
    s += ", ";
    s += y;
    s += ", ";
    s += color;
    s += ", ";
    s += heading;
    return s+")";
  }

}