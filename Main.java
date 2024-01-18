public class Main {
    
    public static class Klass {
        int i = 0;
    };
    
    public static void main(String[] args) {
        Klass klass = new Klass();
        System.out.println("Hello java!\n");
        int i = klass.i;
        if(i == 0) {
            i = 1;
        } else {
            i = 0;
        }
    }
}