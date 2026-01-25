package com.javaweb.bankatm.pojo.DTO;

/**
 * @Classname LoginDTO
 * @Description 用户登录传入信息的Bean
 * @Date 2025/3/17 上午5:00
 * @Created by Lee
 */
public class LoginDTO {
    private String cardNumber;
    private String password;

    public LoginDTO() {
    }

    public LoginDTO(String cardNumber, String password) {
        this.cardNumber = cardNumber;
        this.password = password;
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
    public void setUserCard(String cardNumber) {
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

    public String toString() {
        return "LoginDTO{cardNumber = " + cardNumber + ", password = " + password + "}";
    }
}
