package com.javaweb.bankatm.service;

import com.javaweb.bankatm.pojo.ApiResponse;
import com.javaweb.bankatm.pojo.User;
import jakarta.servlet.http.HttpSession;
import org.springframework.stereotype.Service;

/**
 * @Classname AccountService
 * @Description AccountService服务层的方法定义
 * @Date 2025/3/17 上午4:52
 * @Created by Lee
 */
public interface AccountService {
    /**
     * 获取用户余额
     * @param session
     * @return ApiResponse
     */
    public ApiResponse getUserBalance(HttpSession session);

    public ApiResponse getUserName(HttpSession session);

    /**
     * 用户存款
     * @param amount
     * @param session
     * @return ApiResponse
     */
    public ApiResponse deposit(Double amount, HttpSession session);

    /**
     * 用户取款
     * @param amount
     * @param session
     * @return ApiResponse
     */
    public ApiResponse withdraw(Double amount, HttpSession session);

    /**
     * 用户转账
     * @param target_card_number
     * @param amount
     * @param session
     * @return ApiResponse
     */
    public ApiResponse transfer(String target_card_number, Double amount, HttpSession session);


}
