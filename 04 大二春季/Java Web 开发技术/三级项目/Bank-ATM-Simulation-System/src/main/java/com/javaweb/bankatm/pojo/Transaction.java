package com.javaweb.bankatm.pojo;



/**
 * @Classname Transaction
 * @Description Transaction_tb表类Bean
 * @Date 2025/3/17 上午3:55
 * @Created by Lee
 */
public class Transaction {
    private Integer transactionId;           // 交易ID
    private String userName;                 // 发起用户姓名
    private String userCardNumber;           // 发起用户卡号
    private String type;                     // 交易类型：deposit, withdraw, transfer_in, transfer_out
    private Double amount;                   // 交易金额
    private String targetUserName;           // 目标用户姓名
    private String targetUserCardNumber;     // 目标用户卡号
    private String timestamp;                // 交易时间
    private Boolean transactionStatus;       // 交易状态（true/false）


    public Transaction() {
    }

    public Transaction(Integer transactionId, String userName, String userCardNumber, String type, Double amount, String targetUserName, String targetUserCardNumber, String timestamp, Boolean transactionStatus) {
        this.transactionId = transactionId;
        this.userName = userName;
        this.userCardNumber = userCardNumber;
        this.type = type;
        this.amount = amount;
        this.targetUserName = targetUserName;
        this.targetUserCardNumber = targetUserCardNumber;
        this.timestamp = timestamp;
        this.transactionStatus = transactionStatus;
    }

    /**
     * 获取
     * @return transactionId
     */
    public Integer getTransactionId() {
        return transactionId;
    }

    /**
     * 设置
     * @param transactionId
     */
    public void setTransactionId(Integer transactionId) {
        this.transactionId = transactionId;
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
     * @return userCardNumber
     */
    public String getUserCardNumber() {
        return userCardNumber;
    }

    /**
     * 设置
     * @param userCardNumber
     */
    public void setUserCardNumber(String userCardNumber) {
        this.userCardNumber = userCardNumber;
    }

    /**
     * 获取
     * @return type
     */
    public String getType() {
        return type;
    }

    /**
     * 设置
     * @param type
     */
    public void setType(String type) {
        this.type = type;
    }

    /**
     * 获取
     * @return amount
     */
    public Double getAmount() {
        return amount;
    }

    /**
     * 设置
     * @param amount
     */
    public void setAmount(Double amount) {
        this.amount = amount;
    }

    /**
     * 获取
     * @return targetUserName
     */
    public String getTargetUserName() {
        return targetUserName;
    }

    /**
     * 设置
     * @param targetUserName
     */
    public void setTargetUserName(String targetUserName) {
        this.targetUserName = targetUserName;
    }

    /**
     * 获取
     * @return targetUserCardNumber
     */
    public String getTargetUserCardNumber() {
        return targetUserCardNumber;
    }

    /**
     * 设置
     * @param targetUserCardNumber
     */
    public void setTargetUserCardNumber(String targetUserCardNumber) {
        this.targetUserCardNumber = targetUserCardNumber;
    }

    /**
     * 获取
     * @return timestamp
     */
    public String getTimestamp() {
        return timestamp;
    }

    /**
     * 设置
     * @param timestamp
     */
    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }

    /**
     * 获取
     * @return transactionStatus
     */
    public Boolean getTransactionStatus() {
        return transactionStatus;
    }

    /**
     * 设置
     * @param transactionStatus
     */
    public void setTransactionStatus(Boolean transactionStatus) {
        this.transactionStatus = transactionStatus;
    }

    public String toString() {
        return "Transaction{transactionId = " + transactionId + ", userName = " + userName + ", userCardNumber = " + userCardNumber + ", type = " + type + ", amount = " + amount + ", targetUserName = " + targetUserName + ", targetUserCardNumber = " + targetUserCardNumber + ", timestamp = " + timestamp + ", transactionStatus = " + transactionStatus + "}";
    }
}
