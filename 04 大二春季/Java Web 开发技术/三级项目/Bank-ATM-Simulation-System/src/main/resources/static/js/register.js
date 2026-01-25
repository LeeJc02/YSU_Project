new Vue({
    el: '#app',
    data: {
        register: {
            username: '',
            cardNumber: '',
            password: '',
            confirmPassword: '',
            initialDeposit: 0
        },
        cardError: false, // 卡号错误
        passwordError: false, // 密码错误
        confirmPasswordError: false, // 确认密码错误
        depositError: false, // 存款金额错误
        cardErrorMessage: '卡号格式为16位纯数字',
        passwordErrorMessage: '密码格式为6-8位数字或字母组合',
        confirmPasswordErrorMessage: '密码和确认密码不一致',
        depositErrorMessage: '初始存款金额不能为负数'
    },
    methods: {
        goBack() {
            window.history.back();
        },
        // 卡号格式验证
        checkCardNumber() {
            const cardRegex = /^[0-9]{16}$/;
            if (!cardRegex.test(this.register.cardNumber)) {
                this.cardError = true;
            } else {
                this.cardError = false;
            }
        },
        // 密码格式验证
        checkPassword() {
            const passwordRegex = /^[A-Za-z0-9]{6,8}$/;
            if (!passwordRegex.test(this.register.password)) {
                this.passwordError = true;
            } else {
                this.passwordError = false;
            }
        },
        // 确认密码验证
        checkConfirmPassword() {
            if (this.register.password !== this.register.confirmPassword) {
                this.confirmPasswordError = true;
            } else {
                this.confirmPasswordError = false;
            }
        },
        // 存款金额验证
        checkDeposit() {
            if (this.register.initialDeposit < 0) {
                this.depositError = true;
            } else {
                this.depositError = false;
            }
        },
        // 注册请求
        async registerUser() {
            if (this.cardError || this.passwordError || this.confirmPasswordError || this.depositError) {
                return; // 如果有格式错误，则不提交表单
            }

            try {
                const response = await axios.post('/register', {
                    userName: this.register.username,
                    cardNumber: this.register.cardNumber,
                    password: this.register.password,
                    balance: this.register.initialDeposit
                });
                console.log('注册成功', response.data);
                alert('注册成功！现在可以登录了。');
                window.location.href = '/login';  // 注册成功后跳转到登录页面
            } catch (error) {
                console.error('注册失败', error);
                alert('注册失败，请检查输入的信息。');
            }
        }
    }
});