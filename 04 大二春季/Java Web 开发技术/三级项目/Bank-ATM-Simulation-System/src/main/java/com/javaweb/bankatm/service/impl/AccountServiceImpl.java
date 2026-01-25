package com.javaweb.bankatm.service.impl;

import com.javaweb.bankatm.mapper.TransactionMapper;
import com.javaweb.bankatm.mapper.UserMapper;
import com.javaweb.bankatm.pojo.ApiResponse;
import com.javaweb.bankatm.pojo.User;
import com.javaweb.bankatm.service.AccountService;
import com.javaweb.bankatm.service.TransactionService;
import jakarta.servlet.http.HttpSession;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Service;
import org.springframework.transaction.PlatformTransactionManager;
import org.springframework.transaction.TransactionDefinition;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.transaction.support.DefaultTransactionDefinition;

/**
 * @Classname AccountServiceImpl
 * @Description AccountService服务层的方法实现
 * @Date 2025/3/17 上午4:52
 * @Created by Lee
 */
@Service
public class AccountServiceImpl implements AccountService {
    @Autowired
    private TransactionService transactionService;
    @Autowired
    private PlatformTransactionManager transactionManager;
    @Autowired
    private UserMapper userMapper;
    @Override
    public ApiResponse getUserBalance(HttpSession session) {
        User user = (User)session.getAttribute("currentUser");

        Double balance = userMapper.getBalanceByCardNumber((user.getCardNumber()));

        String message = "查询成功";
        HttpStatus status = HttpStatus.OK;

        return new ApiResponse(status, message, balance);
    }

    @Override
    public ApiResponse getUserName(HttpSession session) {
        User user = (User)session.getAttribute("currentUser");

        return new ApiResponse(HttpStatus.OK, "查询成功", user.getUserName());
    }

    @Override
    public ApiResponse deposit(Double amount, HttpSession session) {
        User user = (User)session.getAttribute("currentUser");
        if(amount < 0) {
            return new ApiResponse(
                    HttpStatus.BAD_REQUEST,
                    "存款失败"
            );
        }

        // 存款并记录存款交易记录
        Boolean transaction_status = false;
        transaction_status = userMapper.depositByCardNumber(user.getCardNumber(), amount);
        transactionService.recordDepositTransaction(user, amount, transaction_status);

        String message;
        HttpStatus status;

        if(transaction_status) {
            message = "存款成功";
            status = HttpStatus.OK;
            user.setBalance(user.getBalance() + amount);
        } else {
            message = "存款失败";
            status = HttpStatus.BAD_REQUEST;
        }

        return new ApiResponse(status, message);
    }

    @Override
    @Transactional
    public ApiResponse withdraw(Double amount, HttpSession session) {
        DefaultTransactionDefinition def = new DefaultTransactionDefinition();
        def.setPropagationBehavior(TransactionDefinition.PROPAGATION_REQUIRED);
        TransactionStatus status = transactionManager.getTransaction(def);

        User user = (User)session.getAttribute("currentUser");
        if (user == null) {
            return new ApiResponse(HttpStatus.BAD_REQUEST, "用户未登录");
        }

        Boolean transaction_status = false;

        try {
            if (user.getBalance() < amount) {
                throw new RuntimeException("取款失败，余额不足");
            }

            // 取款并记录取款交易记录
            transaction_status = userMapper.withdrawByCardNumber(user.getCardNumber(), amount);


            if(transaction_status) {
                user.setBalance(user.getBalance() - amount);
                transactionManager.commit(status);  // 提交事务
                transactionService.recordWithDrawTransaction(user, amount, transaction_status);
                return new ApiResponse(HttpStatus.OK, "取款成功");
            } else {
                throw new RuntimeException("取款失败");
            }

        } catch (Exception e) {
            transactionService.recordWithDrawTransaction(user, amount, transaction_status);
            transactionManager.rollback(status);
            return new ApiResponse(HttpStatus.BAD_REQUEST, e.getMessage());
        }
    }

    @Override
    @Transactional
    public ApiResponse transfer(String target_card_number, Double amount, HttpSession session) {
        // 事件准备
        DefaultTransactionDefinition def = new DefaultTransactionDefinition();
        def.setPropagationBehavior(TransactionDefinition.PROPAGATION_REQUIRED);
        TransactionStatus status = transactionManager.getTransaction(def);

        User user = (User)session.getAttribute("currentUser");
        if (user == null) {
            return new ApiResponse(HttpStatus.BAD_REQUEST, "用户未登录");
        }
        if (amount <= 0) {
            return new ApiResponse(HttpStatus.BAD_REQUEST, "转账金额必须大于零");
        }

        Boolean transaction_status = false;

        try {
            if (user.getBalance() < amount) {
                throw new RuntimeException("转账失败，余额不足");
            }

            // 转账并记录转账记录
            Boolean withdraw_result = userMapper.withdrawByCardNumber(user.getCardNumber(), amount);
            Boolean deposit_result = userMapper.depositByCardNumber(target_card_number, amount);

            transaction_status = withdraw_result && deposit_result;

            if(transaction_status) {
                user.setBalance(user.getBalance() - amount);
                // 事务提交
                transactionManager.commit(status);
                // 记录成功流水
                transactionService.recordWithDrawTransaction(user, amount, transaction_status);
                return new ApiResponse(HttpStatus.OK, "转账成功");
            } else {
                throw new RuntimeException("转账失败");
            }

        } catch (Exception e) {
            if (user != null) {
                // 记录失败流水
                transactionService.recordTransferTransaction(user, target_card_number, amount, transaction_status);
            }
            // 事务回滚
            transactionManager.rollback(status);
            return new ApiResponse(HttpStatus.BAD_REQUEST, e.getMessage());
        }
    }
}
