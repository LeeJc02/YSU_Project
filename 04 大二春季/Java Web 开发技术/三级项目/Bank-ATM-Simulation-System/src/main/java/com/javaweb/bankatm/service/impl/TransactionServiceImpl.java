package com.javaweb.bankatm.service.impl;

import com.javaweb.bankatm.mapper.TransactionMapper;
import com.javaweb.bankatm.mapper.UserMapper;
import com.javaweb.bankatm.pojo.ApiResponse;
import com.javaweb.bankatm.pojo.Transaction;
import com.javaweb.bankatm.pojo.User;
import com.javaweb.bankatm.service.TransactionService;
import com.javaweb.bankatm.util.DateAndStringUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.jdbc.datasource.DataSourceTransactionManager;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.List;

/**
 * @Classname TransactionServiceImpl
 * @Description TransactionService服务层的方法实现
 * @Date 2025/3/17 上午4:52
 * @Created by Lee
 */
@Service
public class TransactionServiceImpl implements TransactionService {
    @Autowired
    private TransactionMapper transactionMapper;
    @Autowired
    private UserMapper userMapper;
    @Autowired
    private DataSourceTransactionManager transactionManager;

    @Override
    public void recordDepositTransaction(User user, Double amount, Boolean transactionStatus) {
        Transaction transaction = new Transaction();

        transaction.setUserName(user.getUserName());
        transaction.setUserCardNumber(user.getCardNumber());
        transaction.setType("deposit");
        transaction.setAmount(amount);
        transaction.setTargetUserName(user.getUserName());
        transaction.setTargetUserCardNumber(user.getCardNumber());
        transaction.setTimestamp(DateAndStringUtil.formatToString());
        transaction.setTransactionStatus(transactionStatus);

        transactionMapper.addTransactionInfo(transactionMapper.getMaxTransactionId() + 1, transaction);
    }

    @Override
    public void recordWithDrawTransaction(User user, Double amount, Boolean transactionStatus) {
        Transaction transaction = new Transaction();

        transaction.setUserName(user.getUserName());
        transaction.setUserCardNumber(user.getCardNumber());
        transaction.setType("withdraw");
        transaction.setAmount(amount);
        transaction.setTargetUserName(user.getUserName());
        transaction.setTargetUserCardNumber(user.getCardNumber());
        transaction.setTimestamp(DateAndStringUtil.formatToString());
        transaction.setTransactionStatus(transactionStatus);

        transactionMapper.addTransactionInfo(transactionMapper.getMaxTransactionId() + 1, transaction);
    }

    @Override
    public void recordTransferTransaction(User user, String target_card_number, Double amount, Boolean transactionStatus) {
        Transaction transaction = new Transaction();
        String target_userName = userMapper.findUserNameByCardNumber(target_card_number);

        transaction.setUserName(user.getUserName());
        transaction.setUserCardNumber(user.getCardNumber());
        transaction.setType("transfer");
        transaction.setAmount(amount);
        transaction.setTargetUserName(target_userName);
        transaction.setTargetUserCardNumber(target_card_number);
        transaction.setTimestamp(DateAndStringUtil.formatToString());
        transaction.setTransactionStatus(transactionStatus);

        transactionMapper.addTransactionInfo(transactionMapper.getMaxTransactionId() + 1, transaction);
    }

    @Override
    public ApiResponse getTransactionAll() {
        List<Transaction> transactions = transactionMapper.getTransactionAll();
        return new ApiResponse(HttpStatus.OK, "查询所有成功", transactions);
    }

    @Override
    public ApiResponse getTransactionByUser(String cardNumber) {
        List<Transaction> transaction = transactionMapper.getTransactionByCardNumber(cardNumber);
        return new ApiResponse(HttpStatus.OK, "查询用户成功", transaction);
    }
}
