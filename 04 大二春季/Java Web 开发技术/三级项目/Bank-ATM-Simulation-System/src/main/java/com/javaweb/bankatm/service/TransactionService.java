package com.javaweb.bankatm.service;

import com.javaweb.bankatm.pojo.ApiResponse;
import com.javaweb.bankatm.pojo.User;

/**
 * @Classname TransactionService
 * @Description TransactionService服务层的方法定义
 * @Date 2025/3/17 上午4:52
 * @Created by Lee
 */
public interface TransactionService {
    /**
     * 提交存款交易记录
     * @param user
     * @param amount
     * @param transactionStatus
     */
    public void recordDepositTransaction(User user, Double amount, Boolean transactionStatus);

    /**
     * 提交取款交易记录
     * @param user
     * @param amount
     * @param transactionStatus
     */
    public void recordWithDrawTransaction(User user, Double amount, Boolean transactionStatus);

    /**
     * 提交转账交易记录
     * @param user
     * @param target_card_number
     * @param amount
     * @param transactionStatus
     */
    public void recordTransferTransaction(User user, String target_card_number, Double amount, Boolean transactionStatus);

    /**
     * 获取全部交易记录
     * @return ApiResponse
     */
    public ApiResponse getTransactionAll();

    /**
     * 获取用户交易记录
     * @param cardNumber
     * @return ApiResponse
     */
    public ApiResponse getTransactionByUser(String cardNumber);
}
