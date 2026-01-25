package com.javaweb.bankatm.pojo.DTO;

import com.javaweb.bankatm.pojo.User;
import com.javaweb.bankatm.util.DateAndStringUtil;

import java.time.LocalDateTime;

/**
 * @Classname RegisterDTO
 * @Description 用户注册传入信息的Bean
 * @Date 2025/3/17 上午5:19
 * @Created by Lee
 */
public class RegisterDTO {
    private String userName;          // 用户名
    private String cardNumber;        // 卡号
    private String password;          // 密码
    private String confirmPassword;   // 确认密码
    private Double balance = 0.00;    // 余额，默认值为 0.00

    public User toUser(){
        User user = new User();
        user.setUserName(userName);
        user.setCardNumber(cardNumber);
        user.setPassword(password);
        user.setBalance(balance);
        user.setCreatedAt(DateAndStringUtil.formatToString());
        return user;
    }

    // Getters and Setters
    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getCardNumber() {
        return cardNumber;
    }

    public void setCardNumber(String cardNumber) {
        this.cardNumber = cardNumber;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getConfirmPassword() {
        return confirmPassword;
    }

    public void setConfirmPassword(String confirmPassword) {
        this.confirmPassword = confirmPassword;
    }

    public Double getBalance() {
        return balance;
    }

    public void setBalance(Double balance) {
        this.balance = balance;
    }
}

