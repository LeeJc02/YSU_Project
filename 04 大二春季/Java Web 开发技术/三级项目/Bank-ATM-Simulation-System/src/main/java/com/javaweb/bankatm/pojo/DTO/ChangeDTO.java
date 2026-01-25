package com.javaweb.bankatm.pojo.DTO;

public class ChangeDTO {
    private String userName;
    private String cardNumber;
    private String password;


    public ChangeDTO() {
    }

    public ChangeDTO(String userName, String cardNumber, String password) {
        this.userName = userName;
        this.cardNumber = cardNumber;
        this.password = password;
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

    public String toString() {
        return "ChangeDTO{userName = " + userName + ", cardNumber = " + cardNumber + ", password = " + password + "}";
    }
}
