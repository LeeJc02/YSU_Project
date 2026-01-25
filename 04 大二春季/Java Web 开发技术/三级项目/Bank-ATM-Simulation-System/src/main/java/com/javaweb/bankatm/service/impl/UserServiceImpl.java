package com.javaweb.bankatm.service.impl;

import com.javaweb.bankatm.mapper.UserMapper;
import com.javaweb.bankatm.pojo.ApiResponse;
import com.javaweb.bankatm.pojo.DTO.ChangeDTO;
import com.javaweb.bankatm.pojo.DTO.LoginDTO;
import com.javaweb.bankatm.pojo.DTO.RegisterDTO;
import com.javaweb.bankatm.pojo.User;
import com.javaweb.bankatm.service.UserService;
import jakarta.servlet.http.HttpSession;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Service;
import org.springframework.transaction.PlatformTransactionManager;
import org.springframework.transaction.TransactionDefinition;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.transaction.support.DefaultTransactionDefinition;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.List;

/**
 * @Classname UserServiceImpl
 * @Description UserService服务层的方法实现
 * @Date 2025/3/17 上午4:52
 * @Created by Lee
 */
@Service
public class UserServiceImpl implements UserService {
    @Autowired
    private UserMapper userMapper;
    @Autowired
    private PlatformTransactionManager transactionManager;

    @Override
    public ApiResponse login(LoginDTO loginDTO, HttpSession session) {
        User user = userMapper.findByCardAndPassword(loginDTO.getCardNumber(), loginDTO.getPassword());
        String message;
        HttpStatus status;

        if(user != null) {
            session.setAttribute("currentUser", user);
            if("root".equals(user.getUserName())) {
                session.setAttribute("role", "admin");
            } else {
                session.setAttribute("role", "user");
            }
            message = "登陆成功";
            status = HttpStatus.OK;
        } else {
            message = "登陆失败";
            status = HttpStatus.UNAUTHORIZED;
        }

        return new ApiResponse(
                status,
                message,
                user
        );
    }

    @Override
    @Transactional
    public ApiResponse registerUser(RegisterDTO userDTO) {
        // 事件配置
        DefaultTransactionDefinition def = new DefaultTransactionDefinition();
        def.setPropagationBehavior(TransactionDefinition.PROPAGATION_REQUIRED);
        TransactionStatus status = transactionManager.getTransaction(def);
        User user = userMapper.findByCardNumber(userDTO.getCardNumber());
        try {
            if("root".equals(userDTO.getPassword()) || user != null) {
                throw new RuntimeException();
            }
            user = userDTO.toUser();
            userMapper.addUserInfo(userMapper.getMaxUserId(), user);
            // 事件提交
            transactionManager.commit(status);
            return new ApiResponse(
                    HttpStatus.CREATED,
                    "注册成功",
                    user
            );
        } catch (Exception e) {
            // 事件回滚
            transactionManager.rollback(status);
            return new ApiResponse(HttpStatus.CONFLICT, "注册失败");
        }
    }

    @Override
    @Transactional
    public ApiResponse deleteUser(HttpSession session) {
        User user = (User) session.getAttribute("currentUser");

        String message;
        HttpStatus status;

        if(user == null || user.getBalance() != 0.00) {
            return new ApiResponse(HttpStatus.UNAUTHORIZED, "无法注销");
        }

        Boolean success = userMapper.deleteByCardAndPassword(user.getCardNumber(), user.getPassword());

        if(success) {
            session.invalidate();
            message = "账号注销成功";
            status = HttpStatus.OK;
        } else {
            message = "注销失败";
            status = HttpStatus.UNAUTHORIZED;
        }

        return new ApiResponse(
                status,
                message
        );
    }

    @Override
    public ApiResponse changeUserInfo(ChangeDTO changeDTO, HttpSession session) {
        User user = (User) session.getAttribute("currentUser");

        String message;
        HttpStatus status;

        User byCardNumber = userMapper.findByCardNumber(changeDTO.getCardNumber());

        Boolean success = false;
        if(byCardNumber == null || changeDTO.getCardNumber().equals(user.getCardNumber())) {
            success = userMapper.changeUserInfo(user.getCardNumber(), changeDTO.getUserName(), changeDTO.getCardNumber(), changeDTO.getPassword());
        }

        if(success) {
            message = "修改成功";
            status = HttpStatus.OK;
            session.setAttribute("currentUser", userMapper.findByCardNumber(changeDTO.getCardNumber()));
        } else {
            message = "修改失败";
            status = HttpStatus.UNAUTHORIZED;
        }

        return new ApiResponse(
                status,
                message,
                user
        );
    }

    @Override
    public ApiResponse getUserAll() {
        List<User> list = userMapper.getUserList();
        return new ApiResponse(
                HttpStatus.OK,
                "查询成功",
                list
        );

    }

    @Override
    public ApiResponse getUser(String cardNumber) {
        return new ApiResponse(
                HttpStatus.OK,
                "查询成功",
                userMapper.findByCardNumber(cardNumber)
        );
    }

}
