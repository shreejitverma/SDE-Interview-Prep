package models;

public class User {
    private final String id;
    private final String firstName;
    private final String lastName;
    private final String bio;
    private final String imageUrl;

    public User(String id, String firstName, String lastName, String bio, String imageUrl) {
        this.id = id;
        this.firstName = firstName;
        this.lastName = lastName;
        this.bio = bio;
        this.imageUrl = imageUrl;
    }
}