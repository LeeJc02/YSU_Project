package com.javaweb.bankatm.pojo;


/**
 * @Classname User
 * @Description User_tb表类Bean
 * @Date 2025/3/17 上午3:55
 * @Created by Lee
 */
public class User {
    private Integer userId;          // 用户ID
    private String userName;         // 用户名
    private String cardNumber;       // 卡号
    private String password;         // 密码
    private Double balance;          // 余额
    private String createdAt;        // 创建时间


    public User() {
    }

    public User(Integer userId, String userName, String cardNumber, String password, Double balance, String createdAt) {
        this.userId = userId;
        this.userName = userName;
        this.cardNumber = cardNumber;
        this.password = password;
        this.balance = balance;
        this.createdAt = createdAt;
    }

    /**
     * 获取
     * @return userId
     */
    public Integer getUserId() {
        return userId;
    }

    /**
     * 设置
     * @param userId
     */
    public void setUserId(Integer userId) {
        this.userId = userId;
    }

    /**
     * 获取
     * @return userName
     */
    public String getUserName() {
        return userName;
    }

    /**
     * 设置
     * @param userName
     */
    public void setUserName(String userName) {
        this.userName = userName;
    }

    /**
     * 获取
     * @return cardNumber
     */
    public String getCardNumber() {
        return cardNumber;
    }

    /**
     * 设置
     * @param cardNumber
     */
    public void setCardNumber(String cardNumber) {
        this.cardNumber = cardNumber;
    }

    /**
     * 获取
     * @return password
     */
    public String getPassword() {
        return password;
    }

    /**
     * 设置
     * @param password
     */
    public void setPassword(String password) {
        this.password = password;
    }

    /**
     * 获取
     * @return balance
     */
    public Double getBalance() {
        return balance;
    }

    /**
     * 设置
     * @param balance
     */
    public void setBalance(Double balance) {
        this.balance = balance;
    }

    /**
     * 获取
     * @return createdAt
     */
    public String getCreatedAt() {
        return createdAt;
    }

    /**
     * 设置
     * @param createdAt
     */
    public void setCreatedAt(String createdAt) {
        this.createdAt = createdAt;
    }

    public String toString() {
        return "User{userId = " + userId + ", userName = " + userName + ", cardNumber = " + cardNumber + ", password = " + password + ", balance = " + balance + ", createdAt = " + createdAt + "}";
    }
}

